/** @package TBTKcalc
 *  @file Magnetization.h
 *  @brief Property container for magnetization
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_MAGNETIZATION
#define COM_DAFER45_TBTK_MAGNETIZATION

#include <complex>

namespace TBTK{
	class CPropertyExtractor;
	class DPropertyExtractor;
	class FileReader;
namespace Property{

/** Container for magnetization. */
class Magnetization{
public:
	/** Constructor. */
	Magnetization(int dimensions, const int* ranges);

	/** Destructor. */
	~Magnetization();

	/** Get the dimension of the magnetization. */
	int getDimensions() const;

	/** Get the ranges for the dimensions of the magnetization. */
	const int* getRanges() const;

	/** Get the number of data elements. */
	int getSize() const;

	/** Get magnetization data. */
	const std::complex<double>* getData() const;
private:
	/** Dimension of the magnetization. */
	int dimensions;

	/** Ranges for the dimensions of the magnetization. */
	int *ranges;

	/** Number of data elements. */
	int size;

	/** Actual data. */
	std::complex<double> *data;

	/** CPropertyExtractor is a friend class to allow it to write
	 *  magnetiation data. */
	friend class TBTK::CPropertyExtractor;

	/** DPropertyExtractor is a friend class to allow it to write
	 *  magnetiation data. */
	friend class TBTK::DPropertyExtractor;

	/** FileReader is a friend class to allow it to write magnetiation
	 *  data. */
	friend class TBTK::FileReader;
};

inline int Magnetization::getDimensions() const{
	return dimensions;
}

inline const int* Magnetization::getRanges() const{
	return ranges;
}

inline int Magnetization::getSize() const{
	return size;
}

inline const std::complex<double>* Magnetization::getData() const{
	return data;
}

};	//End namespace Property
};	//End namespace TBTK

#endif
