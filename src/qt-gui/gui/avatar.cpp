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

#include "gui/avatar.hpp"

#include <stdexcept>
#include <QtCore/QDebug>
#include "gui/worker-thread.hpp"
#include "configuration.hpp"

using namespace CI2CVGui;

static Avatar *avatar = NULL;

Avatar::Avatar(QObject *parent)
: AvatarController(parent),
  currently_selected_index(0)
{
	abstract_avatar = AVATAR::LoadAvatar(applicationConfiguration()->pathToAvatarData().toStdString().c_str());	
	if (!abstract_avatar)
		throw std::runtime_error("Failed to load the AVATAR from file.");
	setAvatarIndex(currently_selected_index);
}

AvatarController *
CI2CVGui::avatarController()
{
	if (!avatar) {
		avatar = new Avatar();
	}
	return avatar;
}

QSize
Avatar::frameSize()
{
	return QSize(0,0);
}

int
Avatar::numberOfAvatars() const
{
	return abstract_avatar->numberOfAvatars();	
}

int
Avatar::avatarIndex() const
{
	return currently_selected_index;
}

QImage
Avatar::avatarThumbnail(int index)
{
	cv::Mat cv_image = abstract_avatar->Thumbnail(index);
	cv::Mat cv_thumbnail = cv_image.clone();
	
	cv::cvtColor(cv_image,cv_thumbnail,CV_BGR2RGB);
		
	QImage rv(cv_thumbnail.datastart,
			  cv_thumbnail.cols,
			  cv_thumbnail.rows,
			  3*cv_thumbnail.cols,
			  QImage::Format_RGB888);
	
	rv.detach();
	return rv.copy();
}

void
Avatar::initialiseUser()
{
	workerThread()->initialiseAvatar();	
}

void
Avatar::setAvatarIndex(int index)
{
	currently_selected_index = index;
	workerThread()->setAvatarIndex(index);
}

void
Avatar::stop()
{
	workerThread()->stopAnimating();
}

void
Avatar::start()
{
	workerThread()->startAnimating();
}

AVATAR::Avatar *
Avatar::avatar() const
{
	return abstract_avatar;
}
