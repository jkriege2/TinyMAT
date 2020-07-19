/*
    Copyright (c) 2008-2020 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/

#define _USE_MATH_DEFINES
#include <iostream>
#include <stdio.h>
#include "tinymatwriter.h"
#include <opencv2/opencv.hpp>

using namespace std;


int main( int /*argc*/, const char* /*argv*/[] ) {
    TinyMATWriterFile* mat=TinyMATWriter_open("test_opencv.mat");
	if (mat) {
		cv::Mat mat_eye_32f=cv::Mat::eye(3, 3, CV_32F);
		TinyMATWriter_writeCVMat(mat, "eye_mat_32f", mat_eye_32f);
		
		cv::Mat mat_eye_16uc1=cv::Mat::eye(10, 10, CV_16UC1);
		TinyMATWriter_writeCVMat(mat, "eye_mat_16uc1", mat_eye_16uc1);

		cv::Vec<double,5> vecd(1,2,3,4,5);
		TinyMATWriter_writeCVVecAsColumn(mat, "vecd_col", vecd);
		TinyMATWriter_writeCVVecAsRow(mat, "vecd_row", vecd);
		
		cv::Point_<int> pnt(-1,-2);
		TinyMATWriter_writeCVPointAsColumn(mat, "pnt_col", pnt);
		TinyMATWriter_writeCVPointAsRow(mat, "pnt_row", pnt);
		
		cv::Point3_<uint64_t> pnt3(1,4,8);
		TinyMATWriter_writeCVPointAsColumn(mat, "pnt3_col", pnt3);
		TinyMATWriter_writeCVPointAsRow(mat, "pnt3_row", pnt3);
		
		cv::Scalar_<uint64_t> scal(1,4,8);
		TinyMATWriter_writeCVScalarAsColumn(mat, "scal_col", scal);
		TinyMATWriter_writeCVScalarAsRow(mat, "scal_row", scal);
		
		cv::Size_<uint8_t> size(0,2);
		TinyMATWriter_writeCVScalarAsColumn(mat, "siz_col", size);
		TinyMATWriter_writeCVScalarAsRow(mat, "siz_row", size);
		
		cv::Rect_<double> rect(0.2,0.3,2.34,3.45);
		TinyMATWriter_writeCVRectAsColumn(mat, "rect_col", rect);
		TinyMATWriter_writeCVRectAsRow(mat, "rect_row", rect);
		
		cv::Range range(0,2);
		TinyMATWriter_writeCVPointAsColumn(mat, "range_col", range);
		TinyMATWriter_writeCVPointAsRow(mat, "range_row", range);
		
		std::vector<cv::Point2d> pnts { cv::Point2d(1,2), cv::Point2d(0,0), cv::Point2d(2,1)} ;
		TinyMATWriter_writeCVPointAsColumn(mat, "pnts_col", pnts);
		TinyMATWriter_writeCVPointAsRow(mat, "pnts_row", pnts);

        TinyMATWriter_close(mat);
	}
    return 0;
}
