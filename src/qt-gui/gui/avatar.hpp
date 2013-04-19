// CSIRO has filed various patents which cover the Software. 

// CSIRO grants to you a license to any patents granted for inventions
// implemented by the Software for academic, research and non-commercial
// use only.

// CSIRO hereby reserves all rights to its inventions implemented by the
// Software and any patents subsequently granted for those inventions
// that are not expressly granted to you.  Should you wish to license the
// patents relating to the Software for commercial use please contact
// CSIRO IP & Licensing, Gautam Tendulkar (gautam.tendulkar@csiro.au) or
// Nick Marsh (nick.marsh@csiro.au)

// This software is provided under the CSIRO OPEN SOURCE LICENSE
// (GPL2) which can be found in the LICENSE file located in the top
// most directory of the source code.

// Copyright CSIRO 2013

#ifndef _CI2CV_GUI_GUI_AVATAR_HPP_
#define _CI2CV_GUI_GUI_AVATAR_HPP_

#include "controllers.hpp"
#include "avatar/Avatar.hpp"

#include <QtCore/QScopedPointer>

namespace CI2CVGui {
	class Avatar : public AvatarController
	{
		Q_OBJECT
		
	public:	
		Avatar(QObject *parent = NULL);
		QSize frameSize();
	  
		int numberOfAvatars() const;
	  
		int avatarIndex() const;
	  
		QImage avatarThumbnail(int index);
		
		AVATAR::Avatar *avatar() const;
			  
	public slots:
		void initialiseUser();		
		void setAvatarIndex(int index);
		
		void stop();
		void start();

	private:
		AVATAR::Avatar *abstract_avatar;
		int currently_selected_index;
  };
}

#endif
