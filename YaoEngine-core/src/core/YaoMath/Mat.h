#pragma once
#include<array>
#include <random>
#include<iostream>
//#define Yao_AI_Matrix 
//#define Yao_Game_Matrix
namespace YaoEngine {
	//游戏矩阵和AI矩阵 编译时期和runtime矩阵分开
#ifdef Yao_Game_Matrix
	template<unsigned int Row,unsigned int Col,class t>
	class Mat {
	public:
		Mat() {
			for (int i = 0;i < Row * Col;++i)
			{
				m_data[i] =static_cast<t>(utill::RandomT<t>(0, 100000000));
			}
		}
		template<class ...Arg>
		Mat(Arg... args) {
			constexpr unsigned int size = sizeof...(args);
			if constexpr (std::is_same_v < arg, Mat<1, Col, t>>&&...&&size==Row)
			{
				int c = 0;
				([&](Arg...a) {
					for (int i = 0; i < Col; i++)
					this->GetData()[c*Col+i] = a.GetData()[i];
					c++;
					}(args),...)
			}
			else if constexpr (std::is_same_v < arg, Mat<Row, 1, t>>&&...&size==Col)
			{
				int c = 0;
				([&](Arg... a) {
				
					for (int i = 0; i < Row; ++i)
					{
						this->GetData()[c + i * Col] = a.GetData()[i];
					}
					}(args), ...)
			}
			else if constexpr (std::is_same_v < arg, t>&... && size <= Row * Col)
			{
				Mat temp;
				int c=0;
				([&](Arg...a) {
					m_data[c] = a->GetData()[c];
					c++;
					}(args), ...)
				this->m_data = temp;
			}
		};
		~Mat() {};
		/*
		0  1  2  3  4		0  1  2 	
		5  6  7  8  9    *  3  4  5
		10 11 12 13 14      6  7  8
							9  10 11
							12 13 14
		*/
		template<unsigned int Row2,unsigned int Col2>
		Mat<Row,Col2, t> operator* (Mat<Row2, Col2, t> other) {
			static_assert(Col == Row2, "Matrix multiplication requires the number of columns in the first matrix to be equal to the number of rows in the second matrix.");
			Mat < Row, Col2, t> result;
			for (int i = 0;i < Row;++i)
			{
				for (int j = 0;j < Col2;++j)
				{
					for (int k = 0;k < Col;++k)
					{
						result.GetData()[i * Col2 + j] = result.GetData()[i * Col2 + j] + this->GetData()[i * Col + k] * other.GetData()[k * Col2 + j];
					}
				}
			}
			return result;
		}
		Mat operator+ (Mat other) {
			Mat result;
			for (int i = 0;i < Row * Col;++i)
			{
				result.GetData()[i] = this->GetData()[i] + other.GetData()[i];
			}
			return result;
		}
		Mat operator*(const t &other)
		{
			Mat result;
			for (int i = 0;i < Row * Col;++i)
			{
				result.GetData()[i] = this->GetData()[i] * other;
			}
			return result;
		}

		std::array <t, Row* Col>& GetData()  { return m_data; }

		void Print()
		{
			for (int i = 0; i < Row; ++i)
			{
				for (int j = 0; j < Col; ++j)
				{
					std::cout << m_data[i * Col + j] << " ";
				}
				std::cout << std::endl;
			}
		}
	private:
		std::array <t,Row* Col> m_data;
	};
	template<unsigned int Row,class t>
	using Colvec = Mat<Row, 1, t>;
	template<unsigned int Col, class t>
	using Rowvec = Mat<1, Col, t>;
#elif Yao_AI_Matrix 1
	template <class any>
	class Mat {
	public:
		static void SetSize(const unsigned int&& Col, const unsigned int&& Row) {
			this->Col = Col;this->Row = Row;
		};
		static void SetSize(const unsigned int&  Col,  const unsigned int& Row) {
			this->Col = Col;this->Row = Row;
		};

		unsigned int Row, Col;
		Mat() {static_assert(Row != 0 && Col != 0);
			for (int i = 0;i < Row * Col;++i)
			{
				m_data[i] = static_cast<t>(utill::RandomDouble(0, 100000000));
			}
		}
		template <class ...arg>
		Mat(arg... args) { 
		static_assert(std::is_same_v< arg, any>&&...&&sizeof...(arg) <= Row * Col);
		(,...)

		};
	};
	
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
			else if constexpr (std::is_floating_point_v<T>)
			{
				std::uniform_real_distribution<T> dist(min, max);
				return dist(gen);
			}
		}
		double RandomDouble(double min, double max);
	}
}
/*
矩阵数据结构行为序列
1个6*5矩阵
m_data[0]  m_data[1]  m_data[2]  m_data[3]  m_data[4]		
m_data[5]  m_data[6]  m_data[7]  m_data[8]  m_data[9]
m_data[10] m_data[11] m_data[12] m_data[13] m_data[14
m_data[15] m_data[16] m_data[17] m_data[18] m_data[19
m_data[21] m_data[22] m_data[23] m_data[24] m_data[25
m_data[26] m_data[27] m_data[28] m_data[29] m_data[30
*/