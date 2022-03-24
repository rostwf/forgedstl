#ifndef FORGED_STL_INTERNAL_PAIR_H_
#define FORGED_STL_INTERNAL_PAIR_H_

namespace forgedstl {

template <typename T1, typename T2>
struct pair {
    typedef T1 first_type;
    typedef T2 second_type;

    T1 first;
    T2 second;
    pair() : first(T1()), second(T2()) { }
    pair(const T1& a, const T2& b) : first(a), second(b) { }

    template <typename U1, typename U2>
    pair(const pair<U1, U2>& p) : first(p.first), second(p.second) { }
};

template <typename T1, typename T2>
inline bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return x.first == y.first && x.second == y.second;
}

template <typename T1, typename T2>
inline bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}

template <typename T1, typename T2>
inline pair<T1, T2> make_pair(const T1& x, const T2& y) {
    return pair<T1, T2>(x, y);
}

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_PAIR_H_
