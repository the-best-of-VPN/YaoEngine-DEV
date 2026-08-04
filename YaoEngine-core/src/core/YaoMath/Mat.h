#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <random>
#include <type_traits>

namespace YaoEngine {

#ifdef Yao_Game_Matrix
	template<unsigned int Row, unsigned int Col, class T>
	class Mat {
	public:
		Mat()
		{
			m_data.fill(T{});
		}

		template<class ...Args, class = std::enable_if_t<(std::is_convertible_v<Args, T> && ...)>>
		Mat(Args... args)
		{
			static_assert(sizeof...(Args) <= Row * Col, "Too many values for matrix");
			m_data.fill(T{});

			if constexpr (sizeof...(Args) > 0)
			{
				T values[] = { static_cast<T>(args)... };
				for (unsigned int i = 0; i < sizeof...(Args); ++i)
					m_data[i] = values[i];
			}
		}

		template<unsigned int Row2, unsigned int Col2>
		Mat<Row, Col2, T> operator*(const Mat<Row2, Col2, T>& other) const
		{
			static_assert(Col == Row2, "Matrix multiplication size mismatch");

			Mat<Row, Col2, T> result;
			for (unsigned int row = 0; row < Row; ++row)
			{
				for (unsigned int col = 0; col < Col2; ++col)
				{
					T value{};
					for (unsigned int k = 0; k < Col; ++k)
						value += m_data[row * Col + k] * other.GetData()[k * Col2 + col];

					result.GetData()[row * Col2 + col] = value;
				}
			}
			return result;
		}

		Mat operator+(const Mat& other) const
		{
			Mat result;
			for (unsigned int i = 0; i < Row * Col; ++i)
				result.GetData()[i] = m_data[i] + other.GetData()[i];
			return result;
		}

		Mat operator-(const Mat& other) const
		{
			Mat result;
			for (unsigned int i = 0; i < Row * Col; ++i)
				result.GetData()[i] = m_data[i] - other.GetData()[i];
			return result;
		}

		Mat operator*(const T& scalar) const
		{
			Mat result;
			for (unsigned int i = 0; i < Row * Col; ++i)
				result.GetData()[i] = m_data[i] * scalar;
			return result;
		}

		T& operator[](unsigned int index) { return m_data[index]; }
		const T& operator[](unsigned int index) const { return m_data[index]; }

		std::array<T, Row * Col>& GetData() { return m_data; }
		const std::array<T, Row * Col>& GetData() const { return m_data; }

		void Print() const
		{
			for (unsigned int row = 0; row < Row; ++row)
			{
				for (unsigned int col = 0; col < Col; ++col)
					std::cout << m_data[row * Col + col] << " ";
				std::cout << std::endl;
			}
		}

		const T* data() const { return m_data.data(); }

	private:
		std::array<T, Row * Col> m_data;
	};

	template<unsigned int Row, class T>
	using Colvec = Mat<Row, 1, T>;

	template<unsigned int Col, class T>
	using Rowvec = Mat<1, Col, T>;

	template<unsigned int Row, class T>
	T Dot(const Mat<Row, 1, T>& left, const Mat<Row, 1, T>& right)
	{
		T result{};
		for (unsigned int i = 0; i < Row; ++i)
			result += left[i] * right[i];
		return result;
	}

	template<class T>
	Mat<4, 1, T> Cross(const Mat<4, 1, T>& left, const Mat<4, 1, T>& right)
	{
		return Mat<4, 1, T>(
			left[1] * right[2] - left[2] * right[1],
			left[2] * right[0] - left[0] * right[2],
			left[0] * right[1] - left[1] * right[0],
			T{}
		);
	}

	template<unsigned int Row, class T>
	Mat<Row, 1, T> Normalize(const Mat<Row, 1, T>& value)
	{
		T length = std::sqrt(Dot(value, value));
		if (length == T{})
			return Mat<Row, 1, T>();

		return value * (static_cast<T>(1) / length);
	}
#endif

	namespace utill {
		template<class T>
		T RandomT(T min, T max)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());

			if constexpr (std::is_integral_v<T>)
			{
				std::uniform_int_distribution<T> dist(min, max);
				return dist(gen);
			}
			else
			{
				std::uniform_real_distribution<T> dist(min, max);
				return dist(gen);
			}
		}

		double RandomDouble(double min, double max);
	}
}
