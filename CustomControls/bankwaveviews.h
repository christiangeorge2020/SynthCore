// -----------------------------------------------------------------------------
//    ASPiK Custom Views File:  bankwaveviews.h
//
/**
    \file   customviews.h
    \author Will Pirkle
    \date   8 -October-2019
    \brief  interface file for bank and wave interactive views
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#pragma once
#include "vstgui/vstgui.h"
#include "vstgui/vstgui_uidescription.h" // for IController

#include "../PluginKernel/pluginstructures.h"

namespace VSTGUI {

// --- messages 
const unsigned int NO_OP = 0; // no message
const unsigned int ADD_BANK_WAVENAMES = 0;	// add a set of wavenames for a bank
const unsigned int UPDATE_BANK_NAMES = 1;	// update/reload the bank names

// --- custom message structure
struct BankWaveMessage
{
	BankWaveMessage() {}
	BankWaveMessage(const BankWaveMessage& initMessage)
	{
		message = initMessage.message;
		waveformNames = initMessage.waveformNames;
		bankNames = initMessage.bankNames;
	}

	BankWaveMessage& operator =(const BankWaveMessage& viewMessage)
	{
		message = viewMessage.message;
		waveformNames = viewMessage.waveformNames;
		bankNames = viewMessage.bankNames;
		return *this;
	}

	// --- show/hide flag
	unsigned int message = NO_OP;
	unsigned int bankIndex = 0;
	std::vector<std::string> waveformNames;
	std::vector<std::string> bankNames;
};

/**
\class BankAndWaveController
\ingroup Custom-SubControllers
\brief
- This object creates bank and waveform connected controls.
- The current version supports only two option menus

\author Will Pirkle http://www.willpirkle.com
\version Revision : 1.0
\date Date : 2019 / 10 / 8
*/
class BankAndWaveController : public IController, public ICustomView /* NOTE: ICustomView is not required, I'm just using it for communcation with plugin */
{
public:
	// --- ICustomView
	virtual void updateView()
	{
		if (bankStrings.size() > 0 && bankControl)
		{
			// --- remove all of the entries from the option menu
			bankControl->removeAllEntry();

			// --- reload the control
			for (int i = 0; i < bankStrings.size(); i++)
			{
				bankControl->addEntry(bankStrings[i].c_str());
			}

			// --- reset this; it is our flag
			bankStrings.clear();

			// --- select first table; need to do this in case of empty table slots!
			//bankControl->setValue(0.0);

			// --- NOTE: call our own handler for this so that it can reload the proper waveform set
			this->valueChanged(bankControl);
		}

	}

	// --- allow plugin to send this object a message
	virtual void sendMessage(void* data)
	{
		// --- host will load a message queue
		BankWaveMessage* viewMessage = static_cast<BankWaveMessage*>(data);
		if (!viewMessage) return;

		if (viewMessage->message == ADD_BANK_WAVENAMES)
		{
			bankWaveStringSets.push_back(viewMessage->waveformNames);
			return; // do not process further
		}
		if (viewMessage->message == UPDATE_BANK_NAMES)
		{
			// --- clear old bank strings
			bankStrings.clear();
			bankStrings = viewMessage->bankNames;
			return; // do not process further
		}

		// --- how to query and respond to this object with text strings and a messageData void*
		//if (viewMessage->queryString.compare("Hello There!") == 0)
		//{
		//	viewMessage->replyString.assign("I'm Here!!");
		//	viewMessage->messageData = this; // <--- passing a pointer back via the messageData value
		//}
		// --- for asyychronous queueing
		// --->> CustomViewMessage has =operator
		// dataQueue->enqueue(*viewMessage);

	}

	/** BankAndWaveController constructor
	\param _parentController - pointer to the parent controller so we can forward messages to it
	*/
	BankAndWaveController(IController* _parentController)
	{
		// --- save the parent listener
		parentController = _parentController;

		// --- create our incoming data-queue (not used right now)
		dataQueue = new moodycamel::ReaderWriterQueue<BankWaveMessage, 32>;
	}

	~BankAndWaveController()
	{
		if (dataQueue) delete dataQueue;
	}


	/** called once per child view of the container that owns the sub-controller this is where we grab and store the
		view objects that we want to link
	\param view - the newly created view to use
	\param attributes - UIAttributes of control
	\param IUIDescription - IUIDescription of control
	\return the verified view
	*/
	virtual CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override
	{
		COptionMenu* menu = dynamic_cast<COptionMenu*>(view);
		const std::string* customViewName = attributes.getAttributeValue(description->kCustomViewName);

		// --- save button, push back knob onto list
		if (menu && customViewName)
		{
			int findIt = customViewName->find("BankMenu");
			if (findIt >= 0)
			{
				// --- we've got the bank option menu
				bankControl = menu;
			}
			findIt = customViewName->find("WaveMenu");
			if (findIt >= 0)
			{
				// --- we've got the wave option menu
				waveControl = menu;
			}
		}

		return view;
	}

	/** called when any control  in the view container changes; we only care about the link button and the knobs, we ignore the others
	view objects that we want to link
	\param control - the control whose value changed
	*/
	virtual void valueChanged(CControl* control) override
	{
		if (control == bankControl)
		{
			// --- do stuff to the wave control
			unsigned int newVal = bankControl->getValue();

			if (newVal >= bankWaveStringSets.size())
			{
				// --- remove all of the entries from the option menu
				waveControl->removeAllEntry();
				waveControl->addEntry("-- empty --");
				waveControl->invalid();
				return parentController->valueChanged(control);
			}

			if (newVal == lastSelectedBank)
				return parentController->valueChanged(control);

			lastSelectedBank = newVal;

			// --- remove all of the entries from the option menu
			waveControl->removeAllEntry();

			// --- get the new set using the control's value (index) to look it up
			std::vector<std::string> strSet = bankWaveStringSets[newVal];

			// --- reload the control
			for (int i = 0; i < strSet.size(); i++)
			{
				waveControl->addEntry(strSet[i].c_str());
			}

			// --- force wave control to re-draw, exposing its new content
			waveControl->invalid();
		}

		// --- for wave control, nothing special to do (???)
		//

		// --- do the value change at parent level, to set on plugin
		parentController->valueChanged(control);
	}

	/** called once per child view of the container - we simply forward the call to the parent listener
	\param attributes - UIAttributes of control
	\param IUIDescription - IUIDescription of control
	\return the verified view
	*/
	virtual CView* createView(const UIAttributes& attributes, const IUIDescription* description) override 
	{
		return parentController->createView(attributes, description); 
	}

	/** called when the user begins to edit a control (mouse click) - we simply forward the call to the parent listener
	\param pControl - the control
	*/
	virtual void controlBeginEdit(CControl* pControl)override { parentController->controlBeginEdit(pControl); }

	/** called when the user ends editing a control (mouse released) - we simply forward the call to the parent listener
	\param pControl - the control
	*/
	virtual void controlEndEdit(CControl* pControl)override { parentController->controlEndEdit(pControl); }

	/** register the control update receiver objects that
	allow us to ultimately bind GUI controls to plugin variables (thread-safe of course)
	\param pControl - the control
	*/
	virtual void controlTagWillChange(CControl* pControl) override
	{
		pControl->setListener(parentController);
		parentController->controlTagWillChange(pControl);
		pControl->setListener(this);
	}

	/** register the control update receiver objects that
	allow us to ultimately bind GUI controls to plugin variables (thread-safe of course)
	\param pControl - the control
	*/
	virtual void controlTagDidChange(CControl* pControl) override
	{
		pControl->setListener(parentController);
		parentController->controlTagDidChange(pControl);
		pControl->setListener(this);
	}

protected:
	// --- the parent controller; we can issue IController commands to it!
	IController* parentController = nullptr; ///< pointer to owning listener

	COptionMenu* bankControl = nullptr; ///< the link button is defined as a CTextButton (by me)
	COptionMenu* waveControl = nullptr; ///< the link button is defined as a CTextButton (by me)
	int lastSelectedBank = -1;

	// --- a vector full of vectors of strings (equivalent to an array of pointers to arrays of various sizes)
	std::vector< std::vector<std::string> > bankWaveStringSets;

	// --- bank names here
	std::vector<std::string> bankStrings;

	// --- lock-free queue for incoming data, sized to 32 in length (not used yet)
	moodycamel::ReaderWriterQueue<BankWaveMessage, 32>* dataQueue = nullptr;
};



}
