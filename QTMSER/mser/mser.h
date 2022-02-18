/*
    this file is copy from https://github.com/idiap/mser and modify code.
*/

#ifndef MSER_H
#define MSER_H

#include <vector>
#include <stdint.h>


class MSER
{
public:
	struct Region
	{
        int level_;
        int pixel_;
        int area_;
        double moments_[5];
        double variation_;
		
		Region(int level = 256, int pixel = 0);
		
	private:
        bool stable_;
        Region * parent_;
        Region * child_;
        Region * next_;
		
		void accumulate(int x, int y);
		void merge(Region * child);
		void detect(int delta, int minArea, int maxArea, double maxVariation, double minDiversity,
					std::vector<Region> & regions);
		void process(int delta, int minArea, int maxArea, double maxVariation);
		bool check(double variation, int area) const;
		void save(double minDiversity, std::vector<Region> & regions);
		
		friend class MSER;
	};

	MSER(int delta = 2, double minArea = 0.0001, double maxArea = 0.5, double maxVariation = 0.5,
		 double minDiversity = 0.33, bool eight = false);
	
	void operator()(const uint8_t * bits, int width, int height, std::vector<Region> & regions);
	
private:
	void processStack(int newPixelGreyLevel, int pixel, std::vector<Region *> & regionStack);
	std::ptrdiff_t doublePool(std::vector<Region *> & regionStack);
	
	int delta_;
	double minArea_;
	double maxArea_;
	double maxVariation_;
	double minDiversity_;
	bool eight_;
	
	std::vector<Region> pool_;
	std::size_t poolIndex_;
};

#endif
