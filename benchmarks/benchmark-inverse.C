/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s


/* Copyright (c) FFLAS-FFPACK
* Written by Clément Pernet <clement.pernet@imag.fr>
* ========LICENCE========
* This file is part of the library FFLAS-FFPACK.
*
* FFLAS-FFPACK is free software: you can redistribute it and/or modify
* it under the terms of the  GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
* ========LICENCE========
*/

#include "fflas-ffpack/fflas-ffpack-config.h"
#include <iostream>
#include <givaro/modular.h>

#include "fflas-ffpack/fflas-ffpack.h"
#include "fflas-ffpack/utils/timer.h"
#include "fflas-ffpack/utils/fflas_io.h"
#include "fflas-ffpack/utils/args-parser.h"


using namespace std;

int main(int argc, char** argv) {
  
	size_t iter = 3;
	int    q    = 131071;
	size_t    n    = 2000;
	std::string file = "";
  
	Argument as[] = {
		{ 'q', "-q Q", "Set the field characteristic (-1 for random).",  TYPE_INT , &q },
		{ 'n', "-n N", "Set the dimension of the matrix.",               TYPE_INT , &n },
		{ 'i', "-i R", "Set number of repetitions.",                     TYPE_INT , &iter },
		{ 'f', "-f FILE", "Set the input file (empty for random).",  TYPE_STR , &file },
		END_OF_ARGUMENTS
	};

	FFLAS::parseArguments(argc,argv,as);

  typedef Givaro::ModularBalanced<double> Field;
  typedef Field::Element Element;

  Field F(q);
  Field::Element * A;

  FFLAS::Timer chrono;
  double time=0.0;

  for (size_t i=0;i<=iter;++i){
	  if (!file.empty()){
		  FFLAS::ReadMatrix (file.c_str(),F,n,n,A);
	  }
	  else {
		  A = FFLAS::fflas_new<Element>(n*n);
		  Field::RandIter G(F);
		  for (size_t j=0; j<(size_t)n*n; ++j)
			  G.random(*(A+j));
	  }

	  int nullity=0;
	  chrono.clear();
	  if (i) chrono.start();
	  FFPACK::Invert (F, n, A, n, nullity);
	  if (i) chrono.stop();

	  time+=chrono.usertime();
	  FFLAS::fflas_delete( A);
  }
  
	// -----------
	// Standard output for benchmark - Alexis Breust 2014/11/14
	#define CUBE(x) ((x)*(x)*(x))
	std::cout << "Time: " << time / double(iter)
			  << " Gfops: " << 2. * CUBE(double(n)/1000.) / time * double(iter);
	FFLAS::writeCommandString(std::cout, as) << std::endl;


  return 0;
}




