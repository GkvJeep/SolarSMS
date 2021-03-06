//---------------------------------------------------------------------------

#ifndef Solar_mainH
#define Solar_mainH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <complex>
#include "liquid/liquid.h"
#include "DataSolar.h"
#include <Registry.hpp>


//---------------------------------------------------------------------------
class TFormIpg : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TOpenDialog *OpenDialog1;
	TEdit *Edit1;
	TButton *Button5;
	TComboBox *ComboBox1;
	TStatusBar *StatusBar1;
	TMemo *LogMemo;
	TButton *Button1;
	TButton *Button2;
	TChart *Chart1;
	TLineSeries *Series1;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Option1;
	TMenuItem *Help1;
	TMenuItem *DataTest1;
	void __fastcall Edit1KeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall LogMemoDblClick(TObject *Sender);
	void __fastcall DataTest1Click(TObject *Sender);
private:	// User declarations
		HANDLE hModem;
		void  __fastcall GetSerialPortsList();
		void __fastcall DataAvailable(TMessage& Message);
		SYSTEMTIME sm;
		DynamicArray<double> Y1,Y2,Y3;
		int cntSer;
        uint32_t smsin,smsout;

public:		// User declarations
	__fastcall TFormIpg(TComponent* Owner);
		AnsiString mFileName;
		TextFile *mFile;

	BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(ESP_EVENT,TMessage,DataAvailable)
	END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormIpg *FormIpg;
//---------------------------------------------------------------------------
#endif
