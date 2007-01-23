/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "Peaker1.h"

using namespace std;
using namespace Marsyas;

Peaker1::Peaker1(string name):MarSystem("Peaker1",name)
{
  rms_ = 0.0;
  winCount_ = 1;
  peakHysterisis_ = 0;
  addControls();
}


Peaker1::~Peaker1()
{
}

MarSystem* 
Peaker1::clone() const 
{
  return new Peaker1(*this);
}

void
Peaker1::addControls()
{
  addctrl("mrs_real/peakSpacing", 0.0);
  addctrl("mrs_real/peakStrength", 0.7);
  addctrl("mrs_natural/peakStart", (mrs_natural)0);
  addctrl("mrs_natural/peakEnd", (mrs_natural)0);
  addctrl("mrs_real/peakGain", 1.0);
  addctrl("mrs_natural/peakStrengthReset", 4);
  addctrl("mrs_real/peakDecay", 0.9);
}




void 
Peaker1::myProcess(realvec& in, realvec& out)
{
   
  mrs_real peakSpacing;
  mrs_real peakStrength;
  mrs_real peakGain;
  
  mrs_natural peakStart;
  mrs_natural peakEnd;
  mrs_natural peakStrengthReset;
  mrs_real    peakDecay;
  

  peakSpacing = getctrl("mrs_real/peakSpacing")->toReal();
  peakStrength = getctrl("mrs_real/peakStrength")->toReal();
  peakStart = getctrl("mrs_natural/peakStart")->toNatural();
  peakEnd = getctrl("mrs_natural/peakEnd")->toNatural();
  peakGain = getctrl("mrs_real/peakGain")->toReal();
  peakStrengthReset = getctrl("mrs_natural/peakStrengthReset")->to<mrs_natural>();
  peakDecay = getctrl("mrs_real/peakDecay")->to<mrs_real>();
  

  out.setval(0.0);

  
  for (o = 0; o < inObservations_; o++)
    {
      peakSpacing = (mrs_real)(peakSpacing * inSamples_);
      mrs_real max;
      mrs_natural maxIndex = 0;
      
      bool peakFound = false;


      for (t=peakStart+1; t < peakEnd-1; t++)
	{
	  if (fabs(in(o,t)) > rms_) 
	    rms_ = fabs(in(o,t));
	}
      
      for (t=peakStart+1; t < peakEnd-1; t++)
	{
	  
	  
	  // peak has to be larger than neighbors 
	  if ((in(o,t -1) < in(o,t)) 
	      && (in(o,t+1) < in(o,t))
	      && (fabs(in(o,t)) > peakStrength * rms_)
	      )
	    {
	      max = in(o,t);
	      maxIndex = t;
	      
	      
	      for (int j=0; j < (mrs_natural)peakSpacing; j++)
		{
		  if (t+j < peakEnd-1)
		    if (in(o,t+j) > max)
		      {
			max = in(o,t+j);
			maxIndex = t+j;
		      }
		}
	      
	      t += (mrs_natural)peakSpacing;
	      if ((peakHysterisis_ > peakStrengthReset) ||
		  (peakHysterisis_ == 0)
		  ) 
		{
		  out(o,maxIndex) = fabs(in(o,maxIndex));	      
		  peakHysterisis_ = 1;
		}
	      
	      rms_ = fabs(in(o,maxIndex));
	      peakFound = true;
	    }
	}
      

      if (!peakFound) 
	rms_ *= peakDecay;
      peakHysterisis_ ++;
    }
  
  
      
}
