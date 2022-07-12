#pragma once

#include <functional>

#undef min

namespace std {
	template<typename T>
	void hash_combine(size_t& seed, T const& v) {
		seed ^= stdext::hash_value(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<class T1, class T2>
	struct hash<pair<T1, T2>> {
		std::size_t operator()(const pair<T1, T2>& p) const noexcept {
			std::size_t seed1(0);
			hash_combine(seed1, p.first);
			hash_combine(seed1, p.second);

			std::size_t seed2(0);
			hash_combine(seed2, p.second);
			hash_combine(seed2, p.first);

			return std::min(seed1, seed2);
		}
	};

	template<class T1, class T2>
	struct less<pair<T1, T2>> {
		bool operator()(const pair<T1, T2>& lp,
			const std::pair<T1, T2>& rp) const {
			return (lp.first == rp.first)
				? less<T2>()(lp.second, rp.second)
				: less<T1>()(lp.first, rp.first);
		}
	};

	template<class T1, class T2>
	struct equal_to<pair<T1, T2>> {
		bool operator()(const pair<T1, T2>& lp,
			const std::pair<T1, T2>& rp) const {
			return equal_to<T1>()(p1.first, p2.first) &&
				   equal_to<T2>()(p1.second, p2.second);
		}
	};
}