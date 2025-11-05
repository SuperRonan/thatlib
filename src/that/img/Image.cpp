#include <that/img/Image.hpp>
#include <that/math/Half.hpp>

#include <that/img/FormatConversion.hpp>

#include <concepts>
#include <functional>
#include <bit>

#ifndef USE_ALLOCA
#define USE_ALLOCA 1
#endif

namespace that
{

	namespace img
	{
		template <class F>
		concept PixelTransformFunction = std::convertible_to<F, std::function<void(const uint8_t*, uint8_t*)>>;

		class ImageProcessor
		{
		public:

			// Are the templates really worth it?
			// The process is probably already memory bounded...


			template <PixelTransformFunction F>
			static constexpr void ProcessPerPixelSameMajor(const uint8_t* src, uint8_t* dst, size_t src_pixel_size, size_t dst_pixel_size, size_t size, F const& f)
			{
				for (size_t i = 0; i < size; ++i)
				{
					const uint8_t* src_pixel = src + i * src_pixel_size;
					uint8_t* dst_pixel = dst + i * dst_pixel_size;
					f(src_pixel, dst_pixel);
				}
			}

			template <bool SRC_ROW_MAJOR, PixelTransformFunction F>
			static constexpr void ProcessPerPixelDiffMajor(const uint8_t* src, uint8_t* dst, size_t src_pixel_size, size_t dst_pixel_size, size_t w, size_t h, F const& f)
			{
				const bool same_buffer = src == dst;
				if (same_buffer)
				{
					const size_t max_pixel_size = std::max(src_pixel_size, dst_pixel_size);
					const size_t min_pixel_size = std::min(src_pixel_size, dst_pixel_size);
					if (w == h && src_pixel_size == dst_pixel_size)
					{
#if USE_ALLOCA
						void* tmp = alloca(src_pixel_size);
#else
						static thread_local std::vector<uint8_t> scratch;
						scratch.resize(max_pixel_size);
						void* tmp = scratch.data();
#endif
						// TODO per block process for better cache hit rate
						const auto process_pixel = [&](size_t i, size_t j)
						{
							const size_t index1 = index<SRC_ROW_MAJOR>(i, j, w, w);
							const size_t index2 = index<!SRC_ROW_MAJOR>(i, j, w, w);
							uint8_t* pixel1 = dst + index1 * src_pixel_size;
							uint8_t* pixel2 = dst + index2 * dst_pixel_size;
							uint8_t * ttmp = static_cast<uint8_t*>(tmp);
							std::memcpy(ttmp, pixel1, src_pixel_size); // pixel1 -> tmp
							f(pixel2, pixel1); // pixel2 -> pixel1
							f(ttmp, pixel2); // tmp (pixel1) -> pixel2
						};
					
						for (size_t y = 0; y < h; ++y)
						{
							for (size_t x = 0; x < y; ++x)
							{
								process_pixel(x, y);
							}
							const size_t yindex = index<SRC_ROW_MAJOR>(y, y, w, w);
							uint8_t* pixel = dst + yindex * dst_pixel_size;
							f(pixel, pixel);
						}

#if USE_ALLOCA
					// There should be a "freea(pixel_size)" to reset the stack pointer, but it appears to be done automatically
#endif
					}
					else
					{
						std::vector<uint8_t> tmp(w * h * src_pixel_size);
						std::memcpy(tmp.data(), src, tmp.size());
						assert(tmp.data() != src);
						ProcessPerPixelDiffMajor<SRC_ROW_MAJOR>(tmp.data(), dst, src_pixel_size, dst_pixel_size, w, h, f);
					}
				}
				else
				{
					// TODO per block process for better cache hit rate
					const auto loop = [&](auto pp)
					{
						if constexpr (SRC_ROW_MAJOR)
						{
							for (size_t i = 0; i < w; ++i)
							{
								for (size_t j = 0; j < h; ++j)
								{
									pp(i, j);
								}
							}
						}
						else
						{
							for (size_t i = 0; i < w; ++i)
							{
								for (size_t j = 0; j < h; ++j)
								{
									pp(i, j);
								}
							}
						}
					};

					const auto process_pixel = [&](size_t i, size_t j)
					{
						const size_t src_index = index<SRC_ROW_MAJOR>(i, j, w, h);
						const size_t dst_index = index<!SRC_ROW_MAJOR>(i, j, w, h);
						const uint8_t* src_pixel = src + src_index * src_pixel_size;
						uint8_t* dst_pixel = dst + dst_index * dst_pixel_size;
						f(src_pixel, dst_pixel);
					};
					loop(process_pixel);
				}
			}

			template <PixelTransformFunction F>
			static constexpr void ProcessPerPixel(const uint8_t* src, uint8_t* dst, size_t src_pixel_size, size_t dst_pixel_size, size_t w, size_t h, bool src_row_major, bool dst_row_major, F const& f)
			{
				if (src_row_major == dst_row_major)
				{
					ProcessPerPixelSameMajor(src, dst, src_pixel_size, dst_pixel_size, w * h, f);
				}
				else
				{
					if (src_row_major)
					{
						ProcessPerPixelDiffMajor<true>(src, dst, src_pixel_size, dst_pixel_size, w, h, f);
					}
					else
					{
						ProcessPerPixelDiffMajor<false>(src, dst, src_pixel_size, dst_pixel_size, w, h, f);
					}
				}
			}

			template <bool SRC_ROW_MAJOR>
			static void TransposeImpl(uint8_t* pixels, size_t pixel_size, size_t w, size_t h)
			{
				if (w == h)
				{
#if USE_ALLOCA
					void * tmp = alloca(pixel_size);
#else
					static thread_local std::vector<uint8_t> scratch;
					scratch.resize(pixel_size);
					void * tmp = scratch.data();
#endif
					auto swap_pixel = [&](size_t i, size_t j)
					{
						const size_t src_index = index<SRC_ROW_MAJOR>(i, j, w, h);
						const size_t dst_index = index<!SRC_ROW_MAJOR>(i, j, h, w);
						uint8_t* src_pixel = pixels + src_index * pixel_size;
						uint8_t* dst_pixel = pixels + dst_index * pixel_size;
						std::memcpy(tmp, src_pixel, pixel_size);
						std::memcpy(src_pixel, dst_pixel, pixel_size);
						std::memcpy(dst_pixel, tmp, pixel_size);
					};
					// TODO per block for better cache hit rate
					for (size_t i = 0; i < w; ++i)
					{
						for (size_t j = 0; j < i; ++j)
						{
							swap_pixel(i, j);
						}
					}
#if USE_ALLOCA
				// There should be a "freea(pixel_size)" to reset the stack pointer, but it appears to be done automatically
#endif
				}
				else
				{
					// We can't just swap pixels
					std::vector<uint8_t> tmp(w * h * pixel_size);
					std::memcpy(tmp.data(), pixels, tmp.size());
					// TODO per block for better cache hit rate
					for (size_t i = 0; i < w; ++i)
					{
						for (size_t j = 0; j < h; ++j)
						{
							const size_t src_index = index<SRC_ROW_MAJOR>(i, j, w, h);
							const size_t dst_index = index<!SRC_ROW_MAJOR>(i, j, h, w);
							uint8_t* src_pixel = pixels + src_index * pixel_size;
							uint8_t* dst_pixel = pixels + dst_index * pixel_size;
							std::memcpy(dst_pixel, tmp.data() + src_index, pixel_size);
						}
					}
				}
			}

			static void Transpose(uint8_t* pixels, size_t pixel_size, size_t w, size_t h, bool src_row_major)
			{
				if (src_row_major)
				{
					TransposeImpl<true>(pixels, pixel_size, w, h);
				}
				else
				{
					TransposeImpl<false>(pixels, pixel_size, w, h);
				}
			}

			struct ConvertParams
			{
				const uint8_t* src;
				uint8_t* dst;
				size_t w;
				size_t h;
				FormatInfo const& src_format;
				FormatInfo const& dst_format;
				bool src_row_major;
				bool dst_row_major;
			};

			template <PixelTransformFunction F>
			static constexpr void ProcessPerPixel(ConvertParams const& params, F const f)
			{
				ProcessPerPixel(params.src, params.dst, params.src_format.pixelSize(), params.dst_format.pixelSize(), params.w, params.h, params.src_row_major, params.dst_row_major, f);
			}

			
			
			template <ElementType src_type, uint32_t src_size, ElementType dst_type, uint32_t dst_size>
			static void ConvertPixel(const uint8_t* src, uint8_t* dst, uint32_t channels, uint32_t zero_channels)
			{
				using SrcType = typename UnderlyingPixelType<src_type, src_size>::type;
				using DstType = typename UnderlyingPixelType<dst_type, dst_size>::type;
				static_assert(!std::is_same<SrcType, void>::value);
				static_assert(!std::is_same<DstType, void>::value);
				const SrcType * typed_src = reinterpret_cast<const SrcType*>(src);
				DstType * typed_dst = reinterpret_cast<DstType*>(dst);

				auto convert_channel = GetConvertPixelChannelFunction<src_type, src_size, dst_type, dst_size>();

				for (uint32_t i = 0; i < channels; ++i)
				{
					convert_channel(typed_src[i], typed_dst[i]);
				}
				if (zero_channels)
				{
					std::memset(typed_dst + channels, 0, zero_channels);
				}
			}

			template <ElementType src_type, uint32_t src_size, ElementType dst_type, uint32_t dst_size>
			static bool ConvertFormatDispatchFinal(ConvertParams const& params)
			{
				const uint32_t src_channels = params.src_format.channels;
				const uint32_t dst_channels = params.dst_format.channels;

				const uint32_t convert_channels = std::min(src_channels, dst_channels);
				const uint32_t zero_channels = (dst_channels > src_channels) ? (dst_channels - src_channels) : 0;

				const auto lambda = [&](const uint8_t* src, uint8_t* dst)
				{
					ConvertPixel<src_type, src_size, dst_type, dst_size>(src, dst, convert_channels, zero_channels);
				};

				ProcessPerPixel(params, lambda);

				return true;
			}

			template <ElementType src_type, uint32_t src_size, ElementType dst_type>
			static bool ConvertFormatDispatch4(ConvertParams const& params)
			{
				bool res = false;
				switch (params.dst_format.elem_size)
				{
					case 1:
					{
						if constexpr (dst_type != ElementType::FLOAT)
						{
							res = ConvertFormatDispatchFinal<src_type, src_size, dst_type, 1>(params);
						}
					}
					break;
					case 2:
					{
						res = ConvertFormatDispatchFinal<src_type, src_size, dst_type, 2>(params);
					}
					break;
					case 4:
					{
						res = ConvertFormatDispatchFinal<src_type, src_size, dst_type, 4>(params);
					}
					break;
					case 8:
					{
						res = ConvertFormatDispatchFinal<src_type, src_size, dst_type, 8>(params);
					}
					break;
				}
				return res;
			}

			template <ElementType src_type, ElementType dst_type>
			static bool ConvertFormatDispatch3(ConvertParams const& params)
			{
				bool res = false;
				switch (params.src_format.elem_size)
				{
					case 1:
					{
						if constexpr (src_type != ElementType::FLOAT)
						{
							res = ConvertFormatDispatch4<src_type, 1, dst_type>(params);
						}
						break;
					}
					case 2:
					{
						res = ConvertFormatDispatch4<src_type, 2, dst_type>(params);
					}
					break;
					case 4:
					{
						res = ConvertFormatDispatch4<src_type, 4, dst_type>(params);
					}
					break;
					case 8:
					{
						res = ConvertFormatDispatch4<src_type, 8, dst_type>(params);
					}
					break;
				}
				return res;
			}

			template <ElementType src_type>
			static bool ConvertFormatDispatch2(ConvertParams const& params)
			{
				bool res = false;
				switch (params.dst_format.type)
				{
				case ElementType::UNORM:
				{
					res = ConvertFormatDispatch3<src_type, ElementType::UNORM>(params);
				}
				break;
				case ElementType::SNORM:
				{
					res = ConvertFormatDispatch3<src_type, ElementType::SNORM>(params);
				}
				break;
				case ElementType::UINT:
				{
					res = ConvertFormatDispatch3<src_type, ElementType::UINT>(params);
				}
				break;
				case ElementType::SINT:
				{
					res = ConvertFormatDispatch3<src_type, ElementType::SINT>(params);
				}
				break;
				case ElementType::sRGB:
				{
					res = ConvertFormatDispatch3<src_type, ElementType::sRGB>(params);
				}
				break;
				case ElementType::FLOAT:
				{
					res = ConvertFormatDispatch3<src_type, ElementType::FLOAT>(params);
				}
				break;
				default:
					res = false;
					break;
				}
				return res;
			}

			static bool ConvertFormatDispatch1(ConvertParams const& params)
			{
				bool res = false;
				switch (params.src_format.type)
				{
				case ElementType::UNORM:
				{
					res = ConvertFormatDispatch2<ElementType::UNORM>(params);
				}
				break;
				case ElementType::SNORM:
				{
					res = ConvertFormatDispatch2<ElementType::SNORM>(params);
				}
				break;
				case ElementType::UINT:
				{
					res = ConvertFormatDispatch2<ElementType::UINT>(params);
				}
				break;
				case ElementType::SINT:
				{
					res = ConvertFormatDispatch2<ElementType::SINT>(params);
				}
				break;
				case ElementType::sRGB:
				{
					res = ConvertFormatDispatch2<ElementType::sRGB>(params);
				}
				break;
				case ElementType::FLOAT:
				{
					res = ConvertFormatDispatch2<ElementType::FLOAT>(params);
				}
				break;
				default:
					res = false;
				break;
				}
				return res;
			}


			// dst can be src
			static Result ConvertFormat(ConvertParams const& params) {

				Result result = Result::Success;
			
				using byte = uint8_t;

				const bool same_buffer = params.src == params.dst;
				const bool same_elem_type = params.src_format.type == params.dst_format.type;
				const bool same_elem_size = params.src_format.elem_size == params.dst_format.elem_size;
				const bool same_channels = params.src_format.channels == params.dst_format.channels;
				const uint32_t src_pixel_size = params.src_format.pixelSize();
				const uint32_t dst_pixel_size = params.dst_format.pixelSize();
				const bool same_pixel_size = src_pixel_size == dst_pixel_size;
				const bool same_major = params.src_row_major == params.dst_row_major;

				const auto type_is_int = [](ElementType type) {return type == ElementType::SINT || type == ElementType::UINT; };
				const auto type_is_norm = [](ElementType type) {return type == ElementType::SNORM || type == ElementType::UNORM; };
				const auto type_is_srgb = [](ElementType type) {return type == ElementType::sRGB;};
				const auto type_is_float = [](ElementType type) {return type == ElementType::FLOAT;};
			
				const auto type_is_any_int = [&](ElementType type) {return type_is_int(type) || type_is_norm(type) || type_is_srgb(type);};
			
				const bool src_int = type_is_int(params.src_format.type);
				const bool dst_int = type_is_int(params.dst_format.type);

				const bool src_norm = type_is_norm(params.src_format.type);
				const bool dst_norm = type_is_norm(params.dst_format.type);

				const bool src_srgb = type_is_srgb(params.src_format.type);
				const bool dst_srgb = type_is_srgb(params.dst_format.type);

				const bool src_float = type_is_float(params.src_format.type);
				const bool dst_float = type_is_float(params.dst_format.type);
			
				const bool require_conversion = [&]() -> bool
				{
					bool res = (!same_elem_type || !same_elem_size);
					// Exclude some (memcpy will do the job)
					if (same_elem_size)
					{
						if (type_is_any_int(params.src_format.type) && dst_int)
						{
							// any int -> int
							res = false;
						}
						if (src_int && type_is_any_int(params.dst_format.type))
						{
							// int -> any int
							res = false;
						}
					}
					return res;
				}();

				const bool is_simple_channel_change = [&]() -> bool
				{
					return !require_conversion && same_elem_size;
				}();

				if (is_simple_channel_change)
				{
					const bool widening = params.src_format.channels < params.dst_format.channels;
					const bool narrowing = params.src_format.channels > params.dst_format.channels;
				
					if (widening)
					{
						auto f = [&](const byte* src_pixel, byte* dst_pixel)
						{
							std::memcpy(dst_pixel, src_pixel, src_pixel_size);
							std::memset(dst_pixel + src_pixel_size, 0, dst_pixel_size - src_pixel_size);
						};
						ProcessPerPixel(params, f);
						result = Result::Success;
					}
					else if(narrowing)
					{
						auto f = [&](const byte* src_pixel, byte* dst_pixel)
						{
							std::memcpy(dst_pixel, src_pixel, dst_pixel_size);
						};
						ProcessPerPixel(params, f);
						result = Result::Success;
					}
					else
					{
						if (!same_buffer)
						{
							if (same_major)
							{
								std::memcpy(params.dst, params.src, params.w * params.h * params.src_format.pixelSize());
								result = Result::Success;
							}
							else
							{
								auto f = [&](const byte* src, byte* dst)
								{
									std::memcpy(dst, src, src_pixel_size);
								};
								ProcessPerPixel(params, f);
								result = Result::Success;
							}
						}
						else
						{
							if (same_major)
							{
								result = Result::Success;
							}
							else
							{
								// A simple transpose();
								result = Result::NotImplemented;

							}
						}
					}
				}
				else if (require_conversion)
				{
					const bool converted = ConvertFormatDispatch1(params);
					if (!converted)
					{
						result = Result::CannotConvertFormat;
					}
				}

				return result;
			}
		};

		Result FormatlessImage::convertFormat(FormatInfo const& src_format, bool src_row_major, FormatInfo const& dst_format, bool dst_row_major)
		{
			Result result = Result::Success;
			{
				const size_t expected_byte_size = size() * src_format.pixelSize();
				if (expected_byte_size < byteSize())
				{
					result = Result::InvalidValue;
					return result;
				}
			}
			if (dst_format.pixelSize() > src_format.pixelSize())
			{
				FormatlessImage old = std::move(*this);
				*this = FormatlessImage(old.width(), old.height(), dst_format.pixelSize());
				ImageProcessor::ConvertParams params{
					.src = old.rawData(),
					.dst = this->rawData(),
					.w = width(),
					.h = height(),
					.src_format = src_format,
					.dst_format = dst_format,
					.src_row_major = src_row_major,
					.dst_row_major = dst_row_major,
				};
				result = ImageProcessor::ConvertFormat(params);
				if (result != Result::Success)
				{
					*this = std::move(old);
				}
			}
			else
			{
				ImageProcessor::ConvertParams params{
					.src = this->rawData(),
					.dst = this->rawData(),
					.w = width(),
					.h = height(),
					.src_format = src_format,
					.dst_format = dst_format,
					.src_row_major = src_row_major,
					.dst_row_major = dst_row_major,
				};
				result = ImageProcessor::ConvertFormat(params);
			}
			return result;
		}

		FormatedImage::FormatedImage(FormatedImage const& img, FormatInfo const& new_format, bool row_major) :
			FormatedImage(img.width(), img.height(), new_format, row_major)
		{
			reFormat(new_format, row_major);
		}

		FormatedImage::FormatedImage(FormatedImage && img, FormatInfo const& new_format, bool row_major) :
			FormatedImage(std::move(img))
		{
			reFormat(new_format, row_major);
		}

		void FormatedImage::setFormat(FormatInfo const& format, bool new_row_major)
		{
			_format = format;
			_row_major = new_row_major;
			_pixel_size = _format.pixelSize();
			resize(_w, _h, _format.pixelSize());
		}

		void FormatedImage::setMajor(bool row_major)
		{
			_row_major = row_major;
		}

		Result FormatedImage::reFormat(FormatInfo const& new_format, bool new_row_major)
		{
			Result result = Result::Success;
			if (new_format.pixelSize() > _format.pixelSize())
			{
				FormatedImage old = std::move(*this);
				*this = FormatedImage(old.width(), old.height(), new_format, new_row_major);
				ImageProcessor::ConvertParams params{
					.src = old.rawData(),
					.dst = this->rawData(),
					.w = width(),
					.h = height(),
					.src_format = old.format(),
					.dst_format = new_format,
					.src_row_major = old.rowMajor(),
					.dst_row_major = new_row_major,
				};
				result = ImageProcessor::ConvertFormat(params);
				if (result != Result::Success)
				{
					*this = std::move(old);
				}
			}
			else
			{
				ImageProcessor::ConvertParams params{
					.src = this->rawData(),
					.dst = this->rawData(),
					.w = width(),
					.h = height(),
					.src_format = _format,
					.dst_format = new_format,
					.src_row_major = _row_major,
					.dst_row_major = new_row_major,
				};
				result = ImageProcessor::ConvertFormat(params);
				if (result == Result::Success)
				{
					setFormat(new_format, new_row_major);
				}
			}
			return result;
		}

		Result FormatedImage::copyReformat(FormatedImage const& src)
		{
			ImageProcessor::ConvertParams params{
				.src = src.rawData(),
				.dst = this->rawData(),
				.w = width(),
				.h = height(),
				.src_format = src.format(),
				.dst_format = _format,
				.src_row_major = src.rowMajor(),
				.dst_row_major = rowMajor(),
			};
			return ImageProcessor::ConvertFormat(params);
		}
	}	

}