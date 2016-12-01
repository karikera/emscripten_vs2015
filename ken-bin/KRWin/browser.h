#pragma once

#include "winx.h"
#include "handle.h"

namespace kr
{

	class EmbeddedBrowser:public Node<Translator>
	{
	public:
		virtual ~EmbeddedBrowser() =0;
		virtual void navigate(TextW _url) =0;
		virtual void setRect(const irect& _rc) =0;
		static EmbeddedBrowser* newInstance(win::Window * pWindow); // ErrorCode
	};


}
