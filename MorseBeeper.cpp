#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "MorseBeeper.h"
#include "GUIFrame.h"

#define IDE_THREAD_UPDATE 1111
#define IDE_THREAD_COMPLETE 1112

class MorseBeeperApp : public wxApp
{
public:
	virtual bool OnInit();
private:
	wxLocale m_locale;
};

class MorseBeeperFrame : public GUIFrame
{
public:
	MorseBeeperFrame() : GUIFrame(NULL)
	{
		Bind(wxEVT_THREAD, &MorseBeeperFrame::SetGauge, this, IDE_THREAD_UPDATE);
		Bind(wxEVT_THREAD, &MorseBeeperFrame::SwitchButton, this, IDE_THREAD_COMPLETE);
	}
	void m_buttonOnButtonClick(wxCommandEvent& event);
	void SetGauge(wxThreadEvent& event);
	void SwitchButton(wxThreadEvent& event);
private:
	void OnQuit(wxCommandEvent& event)
	{
		Destroy();
	}
};

class BeepThread : public wxThread
{
public:
	BeepThread(wxString _str, MorseBeeperFrame* handler) : wxThread()
	{
		str = _str;
		m_pHandler = handler;
	}
private:
	virtual void* Entry();

	wxFrame* m_pHandler;
	wxString str;
};

wxIMPLEMENT_APP(MorseBeeperApp);

bool MorseBeeperApp::OnInit()
{
	wxLocale::AddCatalogLookupPathPrefix(wxT("."));
	m_locale.AddCatalog(wxT("zh_CN"));
	m_locale.Init(wxLANGUAGE_CHINESE_SIMPLIFIED);

	MorseBeeperFrame* frame = new MorseBeeperFrame();
	frame->Show(true);
	return true;
}

void MorseBeeperFrame::m_buttonOnButtonClick(wxCommandEvent& event)
{
	wxString m_text = m_textCtrl->GetValue();
	m_text.MakeUpper();

	m_button->Disable();

	BeepThread* thread = new BeepThread(m_text, this);
	thread->Create();
	thread->Run();
}

void MorseBeeperFrame::SetGauge(wxThreadEvent& event)
{
	m_gauge->SetValue(event.GetInt());
}

void MorseBeeperFrame::SwitchButton(wxThreadEvent& event)
{
	m_button->Enable();
}

void* BeepThread::Entry()
{
	int m_length = str.length();
	for (int i = 0; i < m_length; i++) {
		wxThreadEvent* event = new wxThreadEvent(wxEVT_THREAD, IDE_THREAD_UPDATE);
		event->SetInt(((i + 1) * 100 / m_length));
		wxQueueEvent(m_pHandler->GetEventHandler(), event);
		BeepChar(str[i]);
		Sleep(600);
	}
	wxQueueEvent(m_pHandler->GetEventHandler(), new wxThreadEvent(wxEVT_THREAD, IDE_THREAD_COMPLETE));
	return nullptr;
}
