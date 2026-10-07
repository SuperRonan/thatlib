#include <that/utils/impl/SegmentsUnionImpl.hpp>

#include <deque>
#include <ostream>

namespace that
{
#define DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_PRINT(U, C, S) \
	extern template void SegmentUnion<U, C>::printTo<S>(S&) const;

#define DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_IMPL(U, C)\
	extern template class SegmentUnion<U, C>; \
	DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_PRINT(U, C, std::ostream) \
	DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_PRINT(U, C, std::stringstream)

#define DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_1(U) \
	DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_IMPL(U, std::vector) \
	DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_IMPL(U, std::deque)

#define DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_N(N) \
	DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_1(u##N) \
	DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_1(i##N)

	DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_N(64)
	DECLARE_EXTERN_TEMPLATE_SEGMENT_UNION_N(32)

	void test_Segemnts()
	{
		constexpr Segment<int> s(0, 1);
		//constexpr Segment<
	}
}