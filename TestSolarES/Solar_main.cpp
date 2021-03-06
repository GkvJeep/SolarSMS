//---------------------------------------------------------------------------
#include <windows.h>
#include <iostream>
#include <math.h>
#include <complex>
#include <vcl.h>
#pragma hdrstop
#include "Solar_main.h"

 //---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormIpg *FormIpg;
  //---------------------------------------------------------------------------
__fastcall TFormIpg::TFormIpg(TComponent* Owner)
	: TForm(Owner)
{
  hModem = INVALID_HANDLE_VALUE;
  smsin = smsout = 0;
  GetSerialPortsList();
   	for (int t = 0;t< Chart1->SeriesCount();t++)
										Chart1->Series[t]->Clear();
	Chart1->Axes->Left->Automatic = false;
	Chart1->Axes->Left->Maximum = 200;
	Chart1->Axes->Left->Minimum = 0;
  }

//---------------------------------------------------------------------------
void __fastcall TFormIpg::Edit1KeyPress(TObject *Sender, System::WideChar &Key)
{
   //�����
   if((Key>='0')&&(Key <= '9')) return;
   // ������� <Backspace>
   if(Key == VK_BACK) return;
   // ������� <Enter>
   if (Key == VK_RETURN) {
	 Button1->SetFocus();
			return;
   }

	/*
	 if (Key == ','){
		if ((Edit1->Text).Pos(',') != 0)
				Key = 0; // ����������� ��� ������
			return;
	}
	*/
 // ��������� ������� ���������
	Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormIpg::Button5Click(TObject *Sender)
{
	   if(Button5->Caption == "Open"){
	   //Get number
	   int nCom = StrToInt(
			ComboBox1->Items->Strings[ComboBox1->ItemIndex].Delete(1,3)
	   );
	   //������� �������
	   hModem =  OpenModem(nCom,
	   			  Handle, //current handle
				  -1 //DEBUG_TRACE //0
				);

		if(hModem!= INVALID_HANDLE_VALUE){ //������ ?
		  Button5->Caption = "Close";
		   StatusBar1->Panels->Items[0]->Text=
		   ComboBox1->Items->Strings[ComboBox1->ItemIndex]
				  +" Open";
		  StatusBar1->Panels->Items[1]->Text ="SMS Out:"+UIntToStr(smsout);
		  StatusBar1->Panels->Items[2]->Text ="SMS In:"+UIntToStr(smsin);
		  LogMemo->Lines->Clear();
		}
		Chart1->BottomAxis->Automatic  =true;
		Chart1->Repaint();
	   }
	   else
	   {
		  if(hModem!= INVALID_HANDLE_VALUE)
		  CloseModem(hModem);
		  hModem = INVALID_HANDLE_VALUE;
		  Button5->Caption = "Open";
		  StatusBar1->Panels->Items[0]->Text="Close COM";
	   }
}
//---------------------------------------------------------------------------
 void  __fastcall TFormIpg::GetSerialPortsList()
{
	Button5->Enabled=false;
	AnsiString KeyName = "\\Hardware\\DeviceMap\\SerialComm";
	TStringList *SerialCommValues = new TStringList();
	ComboBox1->Items->Clear();
	TRegistry *Registry = new TRegistry;
	try
	{
		Registry->RootKey = HKEY_LOCAL_MACHINE;
		Registry->OpenKeyReadOnly( KeyName );
		Registry->GetValueNames( SerialCommValues );
		for(int i=0; i<SerialCommValues->Count; i++)
		{
			ComboBox1->Items->Add(Registry->ReadString(SerialCommValues->Strings[i]));
			ComboBox1->ItemIndex=0;
		   //	Form1->StatusBar1->Panels->Items[0]->Text= Registry->ReadString(SerialCommValues->Strings[i]);
		}
	}

	__finally
	{
		delete Registry;
		delete SerialCommValues;

		if (ComboBox1->ItemIndex>=0)
		{
			ComboBox1->Enabled=true;
			Button5->Enabled=true;
		}
	}
}
//======================================================================
void __fastcall TFormIpg::FormClose(TObject *Sender, TCloseAction &Action)
{
   if(hModem!= INVALID_HANDLE_VALUE)
		  CloseModem(hModem);
}
//---------------------------------------------------------------------------
 void __fastcall TFormIpg::DataAvailable(TMessage& Message)
 {
		flx_data inData;
		flx_data  *pCmd;
//		DWORD elm;
		UnicodeString S;
		int8_t text[256];
		  StatusBar1->Panels->Items[2]->Text ="SMS In:"+UIntToStr(++smsin);

	switch(Message.LParam)
	{
		case  FLX_DATA:
		case  RET_STATUS:


		//	elm = Message.WParam;
			GetDataModem(hModem,&inData);
			if(Message.LParam==FLX_DATA){
			 sprintf_s(text,sizeof(text),
			 "[UTC]%02i.%02i.%02i [TR]%5i[MAX]%5i",
			  inData.timeU/10000000,
			 (inData.timeU/100000)%100,
			 (inData.timeU/1000)%100,
			 inData.treshold,
			 inData.max_value);
			LogMemo->Font->Color=clLime;
			LogMemo->Lines->Add(UnicodeString(text));
			}else{
			pCmd =(flx_data *)Message.WParam;
			 sprintf_s(text,sizeof(text),
			 "[UTC]%02i.%02i.%02i [TR]%5i[CUR]%5i<<",
			  pCmd->timeU/10000000,
			 (pCmd->timeU/100000)%100,
			 (pCmd->timeU/1000)%100,
			 pCmd->treshold,
			 pCmd->max_value);
			 LogMemo->Lines->Add(UnicodeString(text));
			 Edit1->Text = UIntToStr((unsigned)pCmd->treshold);
			}

		   {
			for(int i = 0;i<MAX_LEN_DATA*8;i++){
			Chart1->Series[0]->Add(inData.flx[i]*1000.0,"",clTeeColor);
			}
		   }
				// Scroll the Horizontal Axis
		 if(Chart1->BottomAxis->Automatic){
				Chart1->BottomAxis->Automatic  =  false;        // <-- we dont want automatic scaling
		}
				Chart1->BottomAxis->Maximum  =  Series1->XValues->Last();
				Chart1->BottomAxis->Minimum =
				Chart1->BottomAxis->Maximum - MAX_LEN_DATA*8*3;  // we want to see the last 100 points only
		break;
/*
		case  RET_STATUS:
			pCmd =(flx_data *)Message.WParam;
			 sprintf_s(text,sizeof(text),
			 "[UTC]%02i.%02i.%02i [TR]%5i [CUR]%5i <<<",
			  pCmd->timeU/10000000,
			 (pCmd->timeU/100000)%100,
			 (pCmd->timeU/1000)%100,
			 pCmd->treshold,
			 pCmd->max_value);
			 LogMemo->Lines->Add(UnicodeString(text));
			 Edit1->Text = UIntToStr((unsigned)pCmd->treshold);
		break;
  */
	 }
 }
void __fastcall TFormIpg::Button1Click(TObject *Sender)
{
	if(hModem!= INVALID_HANDLE_VALUE){
	int treshold = StrToInt(Edit1->Text);
	 if(treshold > MAX_ESP) treshold = MAX_ESP;
	 if(treshold < 50) treshold = 50;
	 Edit1->Text = IntToStr(treshold);
	 SetTresHold(hModem, (float)treshold);
	 StatusBar1->Panels->Items[1]->Text ="SMS Out:"+UIntToStr(++smsout);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormIpg::Button2Click(TObject *Sender)
{
	if(hModem!= INVALID_HANDLE_VALUE){
		  GetStatusModem(hModem);
		  StatusBar1->Panels->Items[1]->Text ="SMS Out:"+UIntToStr(++smsout);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormIpg::LogMemoDblClick(TObject *Sender)
{
 LogMemo->Lines->Clear();
}
//---------------------------------------------------------------------------


void __fastcall TFormIpg::DataTest1Click(TObject *Sender)
{
          OpenDialog1->DefaultExt = "DataFIle";
  OpenDialog1->FileName = "*.flx";
  OpenDialog1->InitialDir=GetCurrentDir();

	if(mFile){
		delete mFile;
		mFile = NULL;
	}

	if (OpenDialog1->Execute())
	{
	  mFileName = OpenDialog1->FileName;
	  FormIpg->Caption =  mFileName;
	}

}
//---------------------------------------------------------------------------

