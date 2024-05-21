# TinyMAT
A (partly templated) C++ library to handle writing simple Matlab(r) MAT file in Version "MATLAB 5.0" or higher

This library implements a very simple interface to write Matlab MAT file (level 5), as described in http://www.mathworks.de/help/pdf_doc/matlab/matfile_format.pdf

It is licensed under the terms of the GNU Lesser general Public license (LGPL) >=2.1


[![Lates Release](https://img.shields.io/github/v/release/jkriege2/TinyMAT)](https://github.com/jkriege2/TinyMAT/releases)

![Language](https://img.shields.io/github/languages/top/jkriege2/TinyMAT)

[![Documentation](https://img.shields.io/badge/documentation-online-blue)](http://jkriege2.github.io/TinyMAT/index.html)

[![Last Commit](https://img.shields.io/github/last-commit/jkriege2/TinyMAT)](https://github.com/jkriege2/TinyMAT/pulse)
[![Contributors](https://img.shields.io/github/contributors/jkriege2/TinyMAT)](https://github.com/jkriege2/TinyMAT/graphs/contributors)

[![Open Issues](https://img.shields.io/github/issues/jkriege2/TinyMAT)](https://github.com/jkriege2/TinyMAT/issues)
[![Closed Issues](https://img.shields.io/github/issues-closed/jkriege2/TinyMAT)](https://github.com/jkriege2/TinyMAT/issues?q=is%3Aissue+is%3Aclosed)

[![Open PRs](https://img.shields.io/github/issues-pr/jkriege2/TinyMAT)](https://github.com/jkriege2/TinyMAT/pulls)
[![Closed PRs](https://img.shields.io/github/issues-pr-closed/jkriege2/TinyMAT)](https://github.com/jkriege2/TinyMAT/pulls?q=is%3Apr+is%3Aclosed)

[![BUILD-MSVC-CI](https://github.com/jkriege2/TinyMAT/actions/workflows/build_windows.yml/badge.svg)](https://github.com/jkriege2/TinyMAT/actions/workflows/build_windows.yml)
[![BUILD-LINUX-CI](https://github.com/jkriege2/TinyMAT/actions/workflows/build_linux.yml/badge.svg)](https://github.com/jkriege2/TinyMAT/actions/workflows/build_linux.yml)
[![MSVC-CodeAnalysis](https://github.com/jkriege2/TinyMAT/actions/workflows/msvc-codeanalysis.yml/badge.svg)](https://github.com/jkriege2/TinyMAT/actions/workflows/msvc-codeanalysis.yml)
[![Docu-Build](https://github.com/jkriege2/TinyMAT/actions/workflows/build_docs.yml/badge.svg)](https://github.com/jkriege2/TinyMAT/actions/workflows/build_docs.yml)

# Example
The following example code writes some arrays and matrices into a MAT-file:
```C++
TinyMATWriterFile* mat=TinyMATWriter_open("test.mat");
if (mat) {
	// a 3D matrix in row-major
	double mat432[4*3*2]= {
	    1,2,3,
		4,5,6,

		10,20,30,
		40,50,60,

		100,200,300,
		400,500,600,

		1000,2000,3000,
		4000,5000,6000,
	};
	int32_t mat432_size[3] = {3,2,4}; // columns, rows, matrices,...

	// a struct as a map of doubles
	std::map<std::string, double> mp1;
	mp1["x"]=100;
	mp1["y"]=200;
	mp1["z"]=300;
	mp1["longname"]=10000*M_PI;

	TinyMATWriter_writeStruct(mat, "struct1", mp1);
	TinyMATWriter_writeMatrixND_rowmajor(mat, "matrix432", mat432, mat432_size, 3);

	TinyMATWriter_close(mat);
}
```


# Library Bindings

* There exists a plugin for the [CImg image processing library](https://cimg.eu/), that uses TinyMATWriter: https://github.com/dtschump/CImg/blob/master/plugins/tinymatwriter.h .

# Documentation

* library docukentation: https://travis-ci.org/jkriege2/TinyMAT
* API documentation: http://jkriege2.github.io/TinyMAT/group__tinymatwriter.html
* build instructions: http://jkriege2.github.io/TinyMAT/page_buildinstructions.html
* usage instructions:http://jkriege2.github.io/TinyMAT/page_useinstructions.html



## Stargazers over time

[![Stargazers over time](https://starchart.cc/jkriege2/TinyMAT.svg)](https://starchart.cc/jkriege2/TinyMAT)

