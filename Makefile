CRT = -MD
OPTIMIZE = -O2
TYPE = Release
EXT = 
OUTPUT_DIR = bin
OBJ_DIR = obj
WCHAR = ON
MACHINE = -machine:X86

!IF "$(ABI)" == "x64"
MACHINE = -machine:X64
OUTPUT_DIR = bin64
OBJ_DIR = obj64
!ELSE
MACHINE = -machine:X86
OUTPUT_DIR = bin
OBJ_DIR = obj
!ENDIF

!IF "$(TYPE)" == "Debug"
EXT = d
OBJ_DIR = $(OBJ_DIR)\debug
CFLAGS = -DDEBUG -D_DEBUG
CXXFLAGS = -DDEBUG -D_DEBUG
OPTIMIZE = -Od
!ELSE
OBJ_DIR = $(OBJ_DIR)\release
CFLAGS = -DNDEBUG -D_NDEBUG
CXXFLAGS = -DNDEBUG -D_NDEBUG
OPTIMIZE = -O2
!ENDIF

!IF "$(WCHAR)" == "ON"
CFLAGS = $(CFLAGS) -Zc:wchar_t
CXXFLAGS = $(CXXFLAGS) -Zc:wchar_t
!ELSE
CFLAGS = $(CFLAGS) -Zc:wchar_t-
CXXFLAGS = $(CXXFLAGS) -Zc:wchar_t-
!ENDIF


RC = rc.exe
LIB_EXE = lib.exe
ML = ml.exe

CC = cl.exe
CFLAGS = $(CFLAGS) -TC -W3 $(CRT)$(EXT) $(OPTIMIZE) -nologo -Zi -D_WINDOWS -D_UNICODE -DUNICODE -DWIN32 -D_CRT_SECURE_NO_WARNINGS -DQT_NO_DYNAMIC_CAST -EHsc /wd4100 /wd4101 /wd4102 /wd4189 /wd4996 -I. -Iconfig

CXX = cl.exe
CXXFLAGS = $(CXXFLAGS) -TP -W3 $(CRT)$(EXT) $(OPTIMIZE) -nologo -Zi -D_WINDOWS -D_UNICODE -DUNICODE -DWIN32 -D_CRT_SECURE_NO_WARNINGS -DQT_NO_DYNAMIC_CAST -EHsc /wd4100 /wd4101 /wd4102 /wd4189 /wd4996 -I. -Iconfig

LD = link.exe
LIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LDFLAGS = -subsystem:windows $(MACHINE) -nologo -libpath:"$(OUTPUT_DIR)"
LDCONSOLEFLAGS = -subsystem:console $(MACHINE) -nologo -libpath:"$(OUTPUT_DIR)"



UTILITIES = utilities
UTILITIES_DIR = $(UTILITIES)
UTILITIES_SRC = $(UTILITIES_DIR)/src
UTILITIES_FLAGS = -I$(UTILITIES_DIR)/include -I$(UTILITIES_DIR)/include/sobject

UTILITIES_OBJS = $(OBJ_DIR)/$(UTILITIES)/sobject.obj\
				$(OBJ_DIR)/$(UTILITIES)/pugixml.obj\
				$(OBJ_DIR)/$(UTILITIES)/strcpcvt.obj\
				$(OBJ_DIR)/$(UTILITIES)/tstring.obj\
				$(OBJ_DIR)/$(UTILITIES)/gdialpha.obj\
				$(OBJ_DIR)/$(UTILITIES)/soui_mem_wrapper.obj\
				$(OBJ_DIR)/$(UTILITIES)/trace.obj\
				$(OBJ_DIR)/$(UTILITIES)/utilities.obj

SOUI = soui
SOUI_DIR = $(SOUI)
SOUI_SRC = $(SOUI_DIR)/src
SOUI_FLAGS = -I$(UTILITIES_DIR)/include\
			 -I$(SOUI_DIR)\
			 -I$(SOUI_DIR)/include\
			 -I$(SOUI_DIR)/include/activex\
			 -I$(SOUI_DIR)/src/activex\
			 -I$(SOUI_DIR)/include/animator\
			 -I$(SOUI_DIR)/include/control\
			 -I$(SOUI_DIR)/include/core\
			 -I$(SOUI_DIR)/include/event\
			 -I$(SOUI_DIR)/include/helper\
			 -I$(SOUI_DIR)/include/interface\
			 -I$(SOUI_DIR)/include/layout\
			 -I$(SOUI_DIR)/include/res.mgr\
			 -I$(SOUI_DIR)/src/updatelayeredwindow -DSOUI_EXPORTS

SOUI_OBJS = $(OBJ_DIR)/$(SOUI)/SApp.obj $(OBJ_DIR)/$(SOUI)/SAxContainer.obj $(OBJ_DIR)/$(SOUI)/SBStr.obj $(OBJ_DIR)/$(SOUI)/SInterpolatorImpl.obj\
			$(OBJ_DIR)/$(SOUI)/SActiveX.obj $(OBJ_DIR)/$(SOUI)/SCalendar.obj $(OBJ_DIR)/$(SOUI)/SCaption.obj $(OBJ_DIR)/$(SOUI)/SCmnCtrl.obj\
			$(OBJ_DIR)/$(SOUI)/SComboBase.obj $(OBJ_DIR)/$(SOUI)/SComboBox.obj $(OBJ_DIR)/$(SOUI)/SComboView.obj $(OBJ_DIR)/$(SOUI)/SDateTimePicker.obj\
			$(OBJ_DIR)/$(SOUI)/SDropDown.obj $(OBJ_DIR)/$(SOUI)/SHeaderCtrl.obj $(OBJ_DIR)/$(SOUI)/SHotKeyCtrl.obj $(OBJ_DIR)/$(SOUI)/SListbox.obj\
			$(OBJ_DIR)/$(SOUI)/SListCtrl.obj $(OBJ_DIR)/$(SOUI)/SListView.obj $(OBJ_DIR)/$(SOUI)/SMCListView.obj $(OBJ_DIR)/$(SOUI)/SMenuBar.obj\
			$(OBJ_DIR)/$(SOUI)/SMessageBox.obj $(OBJ_DIR)/$(SOUI)/SRealWnd.obj $(OBJ_DIR)/$(SOUI)/SRichEdit.obj $(OBJ_DIR)/$(SOUI)/SScrollbar.obj\
			$(OBJ_DIR)/$(SOUI)/SSliderBar.obj $(OBJ_DIR)/$(SOUI)/SSpinButton.obj $(OBJ_DIR)/$(SOUI)/SSpinButtonCtrl.obj $(OBJ_DIR)/$(SOUI)/SSplitWnd.obj\
			$(OBJ_DIR)/$(SOUI)/STabCtrl.obj $(OBJ_DIR)/$(SOUI)/STileView.obj $(OBJ_DIR)/$(SOUI)/STreeCtrl.obj $(OBJ_DIR)/$(SOUI)/STreeView.obj\
			$(OBJ_DIR)/$(SOUI)/Accelerator.obj $(OBJ_DIR)/$(SOUI)/FocusManager.obj $(OBJ_DIR)/$(SOUI)/SCaret.obj $(OBJ_DIR)/$(SOUI)/SDropTargetDispatcher.obj\
			$(OBJ_DIR)/$(SOUI)/SHostDialog.obj $(OBJ_DIR)/$(SOUI)/shostwnd.obj $(OBJ_DIR)/$(SOUI)/SimpleWnd.obj $(OBJ_DIR)/$(SOUI)/SItemPanel.obj\
			$(OBJ_DIR)/$(SOUI)/SMsgLoop.obj $(OBJ_DIR)/$(SOUI)/SObjectFactory.obj $(OBJ_DIR)/$(SOUI)/SPanel.obj $(OBJ_DIR)/$(SOUI)/SSkin.obj\
			$(OBJ_DIR)/$(SOUI)/SWindowMgr.obj $(OBJ_DIR)/$(SOUI)/Swnd.obj $(OBJ_DIR)/$(SOUI)/SwndContainerImpl.obj $(OBJ_DIR)/$(SOUI)/SwndStyle.obj\
			$(OBJ_DIR)/$(SOUI)/EventSet.obj $(OBJ_DIR)/$(SOUI)/NotifyCenter.obj $(OBJ_DIR)/$(SOUI)/AppDir.obj $(OBJ_DIR)/$(SOUI)/DragWnd.obj\
			$(OBJ_DIR)/$(SOUI)/MemDC.obj $(OBJ_DIR)/$(SOUI)/MenuWndHook.obj $(OBJ_DIR)/$(SOUI)/SDIBHelper.obj $(OBJ_DIR)/$(SOUI)/SListViewItemLocator.obj\
			$(OBJ_DIR)/$(SOUI)/slog.obj $(OBJ_DIR)/$(SOUI)/SMatrix.obj $(OBJ_DIR)/$(SOUI)/SMenu.obj $(OBJ_DIR)/$(SOUI)/SMenuEx.obj $(OBJ_DIR)/$(SOUI)/SScriptTimer.obj\
			$(OBJ_DIR)/$(SOUI)/STileViewItemLocator.obj $(OBJ_DIR)/$(SOUI)/STimerEx.obj $(OBJ_DIR)/$(SOUI)/SToolTip.obj $(OBJ_DIR)/$(SOUI)/SwndFinder.obj\
			$(OBJ_DIR)/$(SOUI)/SGridLayout.obj $(OBJ_DIR)/$(SOUI)/SLayoutSize.obj $(OBJ_DIR)/$(SOUI)/SLinearLayout.obj $(OBJ_DIR)/$(SOUI)/SouiLayout.obj\
			$(OBJ_DIR)/$(SOUI)/SDpiAwareFont.obj $(OBJ_DIR)/$(SOUI)/SFontPool.obj $(OBJ_DIR)/$(SOUI)/SNamedValue.obj $(OBJ_DIR)/$(SOUI)/SObjDefAttr.obj\
			$(OBJ_DIR)/$(SOUI)/SResProvider.obj $(OBJ_DIR)/$(SOUI)/SResProviderMgr.obj $(OBJ_DIR)/$(SOUI)/SSkinPool.obj $(OBJ_DIR)/$(SOUI)/SStylePool.obj\
			$(OBJ_DIR)/$(SOUI)/SUiDef.obj $(OBJ_DIR)/$(SOUI)/SUpdateLayeredWindow.obj

SOUI_RESOURCE = soui-sys-resource
SOUI_RESOURCE_DIR = $(SOUI_RESOURCE)
SOUI_RESOURCE_OBJS = $(OBJ_DIR)/$(SOUI_RESOURCE_DIR)/$(SOUI_RESOURCE).res

ZLIB = zlib
ZLIB_DIR = third-part/$(ZLIB)
ZLIB_OBJS = $(OBJ_DIR)/$(ZLIB)/adler32.obj $(OBJ_DIR)/$(ZLIB)/compress.obj $(OBJ_DIR)/$(ZLIB)/crc32.obj $(OBJ_DIR)/$(ZLIB)/deflate.obj\
			$(OBJ_DIR)/$(ZLIB)/gzio.obj $(OBJ_DIR)/$(ZLIB)/infback.obj $(OBJ_DIR)/$(ZLIB)/inffast.obj $(OBJ_DIR)/$(ZLIB)/inflate.obj\
			$(OBJ_DIR)/$(ZLIB)/inftrees.obj $(OBJ_DIR)/$(ZLIB)/trees.obj $(OBJ_DIR)/$(ZLIB)/uncompr.obj $(OBJ_DIR)/$(ZLIB)/zutil.obj

PNG = png
PNG_DIR = third-part/$(PNG)
PNG_OBJS = $(OBJ_DIR)/$(PNG)/png.obj $(OBJ_DIR)/$(PNG)/pngerror.obj $(OBJ_DIR)/$(PNG)/pngget.obj $(OBJ_DIR)/$(PNG)/pngmem.obj\
			$(OBJ_DIR)/$(PNG)/pngpread.obj $(OBJ_DIR)/$(PNG)/pngread.obj $(OBJ_DIR)/$(PNG)/pngrio.obj $(OBJ_DIR)/$(PNG)/pngrtran.obj\
			$(OBJ_DIR)/$(PNG)/pngrutil.obj $(OBJ_DIR)/$(PNG)/pngset.obj $(OBJ_DIR)/$(PNG)/pngtrans.obj $(OBJ_DIR)/$(PNG)/pngwio.obj\
			$(OBJ_DIR)/$(PNG)/pngwrite.obj $(OBJ_DIR)/$(PNG)/pngwtran.obj $(OBJ_DIR)/$(PNG)/pngwutil.obj

SMILEY = smiley
SMILEY_DIR = third-part/$(SMILEY)
SMILEY_OBJS = $(OBJ_DIR)/$(SMILEY)/SSmileyCtrl.obj

MHOOK = mhook
MHOOK_DIR = third-part/$(MHOOK)
MHOOK_OBJS = $(OBJ_DIR)/$(MHOOK)/cpu.obj $(OBJ_DIR)/$(MHOOK)/disasm.obj $(OBJ_DIR)/$(MHOOK)/disasm_x86.obj $(OBJ_DIR)/$(MHOOK)/mhook.obj $(OBJ_DIR)/$(MHOOK)/misc.obj

GTEST = gtest
GTEST_DIR = third-part/$(GTEST)
GTEST_SRC = $(GTEST_DIR)/src
GTEST_OBJS = $(OBJ_DIR)/$(GTEST)/gtest-all.obj $(OBJ_DIR)/$(GTEST)/gtest_main.obj

LUA = lua-52
LUA_DIR = third-part/$(LUA)
LUA_SRC = $(LUA_DIR)/src
LUA_OBJS = $(OBJ_DIR)/$(LUA)/lua.obj
LUA_LIB_OBJS = $(OBJ_DIR)/$(LUA)/lapi.obj $(OBJ_DIR)/$(LUA)/lauxlib.obj $(OBJ_DIR)/$(LUA)/lbaselib.obj $(OBJ_DIR)/$(LUA)/lbitlib.obj\
				$(OBJ_DIR)/$(LUA)/lcode.obj $(OBJ_DIR)/$(LUA)/lcorolib.obj $(OBJ_DIR)/$(LUA)/lctype.obj $(OBJ_DIR)/$(LUA)/ldblib.obj\
				$(OBJ_DIR)/$(LUA)/ldebug.obj $(OBJ_DIR)/$(LUA)/ldo.obj $(OBJ_DIR)/$(LUA)/ldump.obj $(OBJ_DIR)/$(LUA)/lfunc.obj\
				$(OBJ_DIR)/$(LUA)/lgc.obj $(OBJ_DIR)/$(LUA)/linit.obj $(OBJ_DIR)/$(LUA)/liolib.obj $(OBJ_DIR)/$(LUA)/llex.obj\
				$(OBJ_DIR)/$(LUA)/lmathlib.obj $(OBJ_DIR)/$(LUA)/lmem.obj $(OBJ_DIR)/$(LUA)/loadlib.obj $(OBJ_DIR)/$(LUA)/lobject.obj\
				$(OBJ_DIR)/$(LUA)/lopcodes.obj $(OBJ_DIR)/$(LUA)/loslib.obj $(OBJ_DIR)/$(LUA)/lparser.obj $(OBJ_DIR)/$(LUA)/lstate.obj\
				$(OBJ_DIR)/$(LUA)/lstring.obj $(OBJ_DIR)/$(LUA)/lstrlib.obj $(OBJ_DIR)/$(LUA)/ltable.obj $(OBJ_DIR)/$(LUA)/ltablib.obj\
				$(OBJ_DIR)/$(LUA)/ltm.obj $(OBJ_DIR)/$(LUA)/lundump.obj $(OBJ_DIR)/$(LUA)/lvm.obj $(OBJ_DIR)/$(LUA)/lzio.obj

LUA_COMPILER_OBJS = $(OBJ_DIR)/$(LUA)/luac.obj

JSONCPP = jsoncpp
JSONCPP_DIR = third-part/$(JSONCPP)
JSONCPP_SRC = $(JSONCPP_DIR)/src
JSONCPP_OBJS = $(OBJ_DIR)/$(JSONCPP)/json_reader.obj $(OBJ_DIR)/$(JSONCPP)/json_value.obj $(OBJ_DIR)/$(JSONCPP)/json_writer.obj

SCINTILLA = scintilla
SCINTILLA_DIR = third-part/$(SCINTILLA)
SCINTILLA_HEADER = -I$(SCINTILLA_DIR)/include -I$(SCINTILLA_DIR)/lexlib -I$(SCINTILLA_DIR)/src -I$(SCINTILLA_DIR)/win32
SCINTILLA_OBJS = $(OBJ_DIR)/$(SCINTILLA)/Accessor.obj $(OBJ_DIR)/$(SCINTILLA)/AutoComplete.obj $(OBJ_DIR)/$(SCINTILLA)/CallTip.obj $(OBJ_DIR)/$(SCINTILLA)/CaseConvert.obj\
				$(OBJ_DIR)/$(SCINTILLA)/CaseFolder.obj $(OBJ_DIR)/$(SCINTILLA)/Catalogue.obj $(OBJ_DIR)/$(SCINTILLA)/CellBuffer.obj $(OBJ_DIR)/$(SCINTILLA)/CharClassify.obj\
				$(OBJ_DIR)/$(SCINTILLA)/CharacterCategory.obj $(OBJ_DIR)/$(SCINTILLA)/CharacterSet.obj $(OBJ_DIR)/$(SCINTILLA)/ContractionState.obj $(OBJ_DIR)/$(SCINTILLA)/Decoration.obj\
				$(OBJ_DIR)/$(SCINTILLA)/Document.obj $(OBJ_DIR)/$(SCINTILLA)/Editor.obj $(OBJ_DIR)/$(SCINTILLA)/ExternalLexer.obj $(OBJ_DIR)/$(SCINTILLA)/Indicator.obj\
				$(OBJ_DIR)/$(SCINTILLA)/KeyMap.obj $(OBJ_DIR)/$(SCINTILLA)/LexHTML.obj $(OBJ_DIR)/$(SCINTILLA)/LexerBase.obj $(OBJ_DIR)/$(SCINTILLA)/LexerModule.obj\
				$(OBJ_DIR)/$(SCINTILLA)/LexerNoExceptions.obj $(OBJ_DIR)/$(SCINTILLA)/LexerSimple.obj $(OBJ_DIR)/$(SCINTILLA)/LineMarker.obj $(OBJ_DIR)/$(SCINTILLA)/PerLine.obj\
				$(OBJ_DIR)/$(SCINTILLA)/PlatWin.obj $(OBJ_DIR)/$(SCINTILLA)/PositionCache.obj $(OBJ_DIR)/$(SCINTILLA)/PropSetSimple.obj $(OBJ_DIR)/$(SCINTILLA)/RESearch.obj\
				$(OBJ_DIR)/$(SCINTILLA)/RunStyles.obj $(OBJ_DIR)/$(SCINTILLA)/ScintillaBase.obj $(OBJ_DIR)/$(SCINTILLA)/ScintillaWin.obj $(OBJ_DIR)/$(SCINTILLA)/Selection.obj\
				$(OBJ_DIR)/$(SCINTILLA)/Style.obj $(OBJ_DIR)/$(SCINTILLA)/StyleContext.obj $(OBJ_DIR)/$(SCINTILLA)/UniConversion.obj $(OBJ_DIR)/$(SCINTILLA)/ViewStyle.obj\
				$(OBJ_DIR)/$(SCINTILLA)/WordList.obj $(OBJ_DIR)/$(SCINTILLA)/XPM.obj

7Z = 7z
7Z_DIR = third-part/$(7Z)
7Z_FLAGS = -DEXTERNAL_CODECS -DNO_REGISTRY -D_7ZIP_LARGE_PAGES -D_7ZIP_ST_9
7Z_OBJS = $(OBJ_DIR)/$(7Z)/7zAes.obj $(OBJ_DIR)/$(7Z)/7zBuf2.obj $(OBJ_DIR)/$(7Z)/7zCompressionMode.obj $(OBJ_DIR)/$(7Z)/7zCrc.obj\
			$(OBJ_DIR)/$(7Z)/7zDecode.obj $(OBJ_DIR)/$(7Z)/7zEncode.obj $(OBJ_DIR)/$(7Z)/7zExtract.obj $(OBJ_DIR)/$(7Z)/7zFolderInStream.obj\
			$(OBJ_DIR)/$(7Z)/7zHandler.obj $(OBJ_DIR)/$(7Z)/7zHandlerOut.obj $(OBJ_DIR)/$(7Z)/7zHeader.obj $(OBJ_DIR)/$(7Z)/7zIn.obj\
			$(OBJ_DIR)/$(7Z)/7zOut.obj $(OBJ_DIR)/$(7Z)/7zProperties.obj $(OBJ_DIR)/$(7Z)/7zSpecStream.obj $(OBJ_DIR)/$(7Z)/7zStream.obj\
			$(OBJ_DIR)/$(7Z)/7zUpdate.obj $(OBJ_DIR)/$(7Z)/Aes.obj $(OBJ_DIR)/$(7Z)/Alloc.obj $(OBJ_DIR)/$(7Z)/ArchiveExports.obj\
			$(OBJ_DIR)/$(7Z)/ArchiveExtractCallback.obj $(OBJ_DIR)/$(7Z)/ArchiveExtractCallbackMemory.obj $(OBJ_DIR)/$(7Z)/ArchiveOpenCallback.obj $(OBJ_DIR)/$(7Z)/ArchiveUpdateCallback.obj\
			$(OBJ_DIR)/$(7Z)/BZip2Crc.obj $(OBJ_DIR)/$(7Z)/BZip2Decoder.obj $(OBJ_DIR)/$(7Z)/BZip2Encoder.obj $(OBJ_DIR)/$(7Z)/Bcj2.obj\
			$(OBJ_DIR)/$(7Z)/Bcj2Coder.obj $(OBJ_DIR)/$(7Z)/Bcj2Enc.obj $(OBJ_DIR)/$(7Z)/BcjCoder.obj $(OBJ_DIR)/$(7Z)/BitlDecoder.obj\
			$(OBJ_DIR)/$(7Z)/Blake2s.obj $(OBJ_DIR)/$(7Z)/BlobBuffer.obj $(OBJ_DIR)/$(7Z)/Bra.obj $(OBJ_DIR)/$(7Z)/Bra86.obj\
			$(OBJ_DIR)/$(7Z)/BraIA64.obj $(OBJ_DIR)/$(7Z)/BranchMisc.obj $(OBJ_DIR)/$(7Z)/BwtSort.obj $(OBJ_DIR)/$(7Z)/CRC.obj\
			$(OBJ_DIR)/$(7Z)/CWrappers.obj $(OBJ_DIR)/$(7Z)/CabBlockInStream.obj $(OBJ_DIR)/$(7Z)/CabHandler.obj $(OBJ_DIR)/$(7Z)/CabHeader.obj\
			$(OBJ_DIR)/$(7Z)/CabIn.obj $(OBJ_DIR)/$(7Z)/ChmIn.obj $(OBJ_DIR)/$(7Z)/CodecExports.obj $(OBJ_DIR)/$(7Z)/CoderMixer2.obj\
			$(OBJ_DIR)/$(7Z)/CopyCoder.obj $(OBJ_DIR)/$(7Z)/CpuArch.obj $(OBJ_DIR)/$(7Z)/CreateCoder.obj $(OBJ_DIR)/$(7Z)/DeflateDecoder.obj\
			$(OBJ_DIR)/$(7Z)/DeflateEncoder.obj $(OBJ_DIR)/$(7Z)/DeflateProps.obj $(OBJ_DIR)/$(7Z)/Delta.obj $(OBJ_DIR)/$(7Z)/DllExports2.obj\
			$(OBJ_DIR)/$(7Z)/DllSecur.obj $(OBJ_DIR)/$(7Z)/DummyOutStream.obj $(OBJ_DIR)/$(7Z)/DynLimBuf.obj $(OBJ_DIR)/$(7Z)/ErpExcept.obj\
			$(OBJ_DIR)/$(7Z)/FileDir.obj $(OBJ_DIR)/$(7Z)/FileFind.obj $(OBJ_DIR)/$(7Z)/FileIO.obj $(OBJ_DIR)/$(7Z)/FileName.obj\
			$(OBJ_DIR)/$(7Z)/FileStream.obj $(OBJ_DIR)/$(7Z)/FileStreamMemory.obj $(OBJ_DIR)/$(7Z)/FileSys.obj $(OBJ_DIR)/$(7Z)/FilterCoder.obj\
			$(OBJ_DIR)/$(7Z)/FindSignature.obj $(OBJ_DIR)/$(7Z)/GUIDs.obj $(OBJ_DIR)/$(7Z)/HandlerCont.obj $(OBJ_DIR)/$(7Z)/HandlerOut.obj\
			$(OBJ_DIR)/$(7Z)/HmacSha1.obj $(OBJ_DIR)/$(7Z)/HmacSha256.obj $(OBJ_DIR)/$(7Z)/HuffEnc.obj $(OBJ_DIR)/$(7Z)/ImplodeDecoder.obj\
			$(OBJ_DIR)/$(7Z)/ImplodeHuffmanDecoder.obj $(OBJ_DIR)/$(7Z)/InBuffer.obj $(OBJ_DIR)/$(7Z)/InOutTempBuffer.obj $(OBJ_DIR)/$(7Z)/InStreamWithCRC.obj\
			$(OBJ_DIR)/$(7Z)/InStreamWrapper.obj $(OBJ_DIR)/$(7Z)/IntToString.obj $(OBJ_DIR)/$(7Z)/IsoHandler.obj $(OBJ_DIR)/$(7Z)/IsoHeader.obj\
			$(OBJ_DIR)/$(7Z)/IsoIn.obj $(OBJ_DIR)/$(7Z)/ItemNameUtils.obj $(OBJ_DIR)/$(7Z)/LimitedStreams.obj $(OBJ_DIR)/$(7Z)/LzFind.obj\
			$(OBJ_DIR)/$(7Z)/LzFindMt.obj $(OBJ_DIR)/$(7Z)/LzOutWindow.obj $(OBJ_DIR)/$(7Z)/LzhDecoder.obj $(OBJ_DIR)/$(7Z)/Lzma2Dec.obj\
			$(OBJ_DIR)/$(7Z)/Lzma2Decoder.obj $(OBJ_DIR)/$(7Z)/Lzma2Enc.obj $(OBJ_DIR)/$(7Z)/Lzma2Encoder.obj $(OBJ_DIR)/$(7Z)/LzmaDec.obj\
			$(OBJ_DIR)/$(7Z)/LzmaDecoder.obj $(OBJ_DIR)/$(7Z)/LzmaEnc.obj $(OBJ_DIR)/$(7Z)/LzmaEncoder.obj $(OBJ_DIR)/$(7Z)/LzmsDecoder.obj\
			$(OBJ_DIR)/$(7Z)/LzxDecoder.obj $(OBJ_DIR)/$(7Z)/MemBlocks.obj $(OBJ_DIR)/$(7Z)/MethodId.obj $(OBJ_DIR)/$(7Z)/MethodProps.obj\
			$(OBJ_DIR)/$(7Z)/MtCoder.obj $(OBJ_DIR)/$(7Z)/MultiStream.obj $(OBJ_DIR)/$(7Z)/MyAes.obj $(OBJ_DIR)/$(7Z)/MyMap.obj\
			$(OBJ_DIR)/$(7Z)/MyString.obj $(OBJ_DIR)/$(7Z)/MyVector.obj $(OBJ_DIR)/$(7Z)/MyWindows.obj $(OBJ_DIR)/$(7Z)/MyXml.obj\
			$(OBJ_DIR)/$(7Z)/NewHandler.obj $(OBJ_DIR)/$(7Z)/NsisDecode.obj $(OBJ_DIR)/$(7Z)/NsisHandler.obj $(OBJ_DIR)/$(7Z)/NsisIn.obj\
			$(OBJ_DIR)/$(7Z)/OffsetStream.obj $(OBJ_DIR)/$(7Z)/OutBuffer.obj $(OBJ_DIR)/$(7Z)/OutMemStream.obj $(OBJ_DIR)/$(7Z)/OutStreamWithCRC.obj\
			$(OBJ_DIR)/$(7Z)/OutStreamWithSha1.obj $(OBJ_DIR)/$(7Z)/OutStreamWrapper.obj $(OBJ_DIR)/$(7Z)/OutStreamWrapperMemory.obj $(OBJ_DIR)/$(7Z)/ParseProperties.obj\
			$(OBJ_DIR)/$(7Z)/PathScanner.obj $(OBJ_DIR)/$(7Z)/Pbkdf2HmacSha1.obj $(OBJ_DIR)/$(7Z)/Ppmd7.obj $(OBJ_DIR)/$(7Z)/Ppmd7Dec.obj\
			$(OBJ_DIR)/$(7Z)/Ppmd7Enc.obj $(OBJ_DIR)/$(7Z)/Ppmd8.obj $(OBJ_DIR)/$(7Z)/Ppmd8Dec.obj $(OBJ_DIR)/$(7Z)/Ppmd8Enc.obj\
			$(OBJ_DIR)/$(7Z)/PpmdDecoder.obj $(OBJ_DIR)/$(7Z)/PpmdEncoder.obj $(OBJ_DIR)/$(7Z)/PpmdZip.obj $(OBJ_DIR)/$(7Z)/ProgressMt.obj\
			$(OBJ_DIR)/$(7Z)/ProgressUtils.obj $(OBJ_DIR)/$(7Z)/PropId.obj $(OBJ_DIR)/$(7Z)/PropVariant.obj $(OBJ_DIR)/$(7Z)/PropVariant2.obj\
			$(OBJ_DIR)/$(7Z)/PropVariantUtils.obj $(OBJ_DIR)/$(7Z)/QuantumDecoder.obj $(OBJ_DIR)/$(7Z)/RandGen.obj $(OBJ_DIR)/$(7Z)/Rar1Decoder.obj\
			$(OBJ_DIR)/$(7Z)/Rar20Crypto.obj $(OBJ_DIR)/$(7Z)/Rar2Decoder.obj $(OBJ_DIR)/$(7Z)/Rar3Decoder.obj $(OBJ_DIR)/$(7Z)/Rar3Vm.obj\
			$(OBJ_DIR)/$(7Z)/Rar5Aes.obj $(OBJ_DIR)/$(7Z)/Rar5Decoder.obj $(OBJ_DIR)/$(7Z)/RarAes.obj $(OBJ_DIR)/$(7Z)/SevenZipArchive.obj\
			$(OBJ_DIR)/$(7Z)/SevenZipCompressor.obj $(OBJ_DIR)/$(7Z)/SevenZipException.obj $(OBJ_DIR)/$(7Z)/SevenZipExtractor.obj $(OBJ_DIR)/$(7Z)/SevenZipExtractorMemory.obj\
			$(OBJ_DIR)/$(7Z)/SevenZipLister.obj $(OBJ_DIR)/$(7Z)/Sha1.obj $(OBJ_DIR)/$(7Z)/Sha256.obj $(OBJ_DIR)/$(7Z)/ShrinkDecoder.obj\
			$(OBJ_DIR)/$(7Z)/Sort.obj $(OBJ_DIR)/$(7Z)/StreamBinder.obj $(OBJ_DIR)/$(7Z)/StreamObjects.obj $(OBJ_DIR)/$(7Z)/StreamUtils.obj\
			$(OBJ_DIR)/$(7Z)/StringConvert.obj $(OBJ_DIR)/$(7Z)/StringToInt.obj $(OBJ_DIR)/$(7Z)/Synchronization.obj $(OBJ_DIR)/$(7Z)/System.obj\
			$(OBJ_DIR)/$(7Z)/TarHandler.obj $(OBJ_DIR)/$(7Z)/TarHandlerOut.obj $(OBJ_DIR)/$(7Z)/TarHeader.obj $(OBJ_DIR)/$(7Z)/TarIn.obj\
			$(OBJ_DIR)/$(7Z)/TarOut.obj $(OBJ_DIR)/$(7Z)/TarUpdate.obj $(OBJ_DIR)/$(7Z)/Threads.obj $(OBJ_DIR)/$(7Z)/TimeUtils.obj\
			$(OBJ_DIR)/$(7Z)/UTFConvert.obj $(OBJ_DIR)/$(7Z)/UdfIn.obj $(OBJ_DIR)/$(7Z)/UniqBlocks.obj $(OBJ_DIR)/$(7Z)/UsefulFunctions.obj\
			$(OBJ_DIR)/$(7Z)/VirtThread.obj $(OBJ_DIR)/$(7Z)/Wildcard.obj $(OBJ_DIR)/$(7Z)/WimHandler.obj $(OBJ_DIR)/$(7Z)/WimHandlerOut.obj\
			$(OBJ_DIR)/$(7Z)/WimIn.obj $(OBJ_DIR)/$(7Z)/WzAes.obj $(OBJ_DIR)/$(7Z)/XpressDecoder.obj $(OBJ_DIR)/$(7Z)/Xz.obj\
			$(OBJ_DIR)/$(7Z)/XzCrc64.obj $(OBJ_DIR)/$(7Z)/XzCrc64Opt.obj $(OBJ_DIR)/$(7Z)/XzDec.obj $(OBJ_DIR)/$(7Z)/XzEnc.obj\
			$(OBJ_DIR)/$(7Z)/XzIn.obj $(OBJ_DIR)/$(7Z)/ZDecoder.obj $(OBJ_DIR)/$(7Z)/ZipAddCommon.obj $(OBJ_DIR)/$(7Z)/ZipCrypto.obj\
			$(OBJ_DIR)/$(7Z)/ZipHandler.obj $(OBJ_DIR)/$(7Z)/ZipHandlerOut.obj $(OBJ_DIR)/$(7Z)/ZipIn.obj $(OBJ_DIR)/$(7Z)/ZipItem.obj\
			$(OBJ_DIR)/$(7Z)/ZipOut.obj $(OBJ_DIR)/$(7Z)/ZipStrong.obj $(OBJ_DIR)/$(7Z)/ZipUpdate.obj $(OBJ_DIR)/$(7Z)/ZlibDecoder.obj\
			$(OBJ_DIR)/$(7Z)/ZlibEncoder.obj $(OBJ_DIR)/$(7Z)/7zCrcOpt.obj $(OBJ_DIR)/$(7Z)/AesOpt.obj

SKIA = skia
SKIA_DIR = third-part/$(SKIA)
SKIA_SRC = $(SKIA_DIR)/src
SKIA_FLAGS = -I$(SKIA_DIR)/include/config -I$(SKIA_DIR)/include/core -I$(SKIA_DIR)/src/core -I$(SKIA_DIR)/include/gpu\
				-I$(SKIA_DIR)/src/gpu -I$(SKIA_DIR)/include/images -I$(SKIA_DIR)/include/utils -I$(SKIA_DIR)/include/utils\win\
				-I$(SKIA_DIR)/src/utils -I$(SKIA_DIR)/include/ports -I$(SKIA_DIR)/include/effects -I$(SKIA_DIR)/src/sfnt\
				-I$(SKIA_DIR)/include/pathops -I$(SKIA_DIR)/src/image -I$(SKIA_DIR)/src/opts -I$(SKIA_DIR)/src/lazy /wd4244 -DSK_IGNORE_ETC1_SUPPORT

SKIA_OBJS = $(OBJ_DIR)/$(SKIA)/GrAAConvexPathRenderer.obj $(OBJ_DIR)/$(SKIA)/GrAAHairLinePathRenderer.obj $(OBJ_DIR)/$(SKIA)/GrAARectRenderer.obj\
			$(OBJ_DIR)/$(SKIA)/GrAddPathRenderers_default.obj $(OBJ_DIR)/$(SKIA)/GrAllocPool.obj $(OBJ_DIR)/$(SKIA)/GrAtlas.obj $(OBJ_DIR)/$(SKIA)/GrBezierEffect.obj\
			$(OBJ_DIR)/$(SKIA)/GrBicubicEffect.obj $(OBJ_DIR)/$(SKIA)/GrBitmapTextContext.obj $(OBJ_DIR)/$(SKIA)/GrBlend.obj $(OBJ_DIR)/$(SKIA)/GrBufferAllocPool.obj\
			$(OBJ_DIR)/$(SKIA)/GrBufferObj.obj $(OBJ_DIR)/$(SKIA)/GrCacheID.obj $(OBJ_DIR)/$(SKIA)/GrClipData.obj $(OBJ_DIR)/$(SKIA)/GrClipMaskCache.obj\
			$(OBJ_DIR)/$(SKIA)/GrClipMaskManager.obj $(OBJ_DIR)/$(SKIA)/GrConfigConversionEffect.obj $(OBJ_DIR)/$(SKIA)/GrContext.obj $(OBJ_DIR)/$(SKIA)/GrConvexPolyEffect.obj\
			$(OBJ_DIR)/$(SKIA)/GrConvolutionEffect.obj $(OBJ_DIR)/$(SKIA)/GrCustomCoordsTextureEffect.obj $(OBJ_DIR)/$(SKIA)/GrDashingEffect.obj $(OBJ_DIR)/$(SKIA)/GrDebugGL.obj\
			$(OBJ_DIR)/$(SKIA)/GrDefaultPathRenderer.obj $(OBJ_DIR)/$(SKIA)/GrDistanceFieldTextContext.obj $(OBJ_DIR)/$(SKIA)/GrDistanceFieldTextureEffect.obj\
			$(OBJ_DIR)/$(SKIA)/GrDitherEffect.obj $(OBJ_DIR)/$(SKIA)/GrDrawState.obj $(OBJ_DIR)/$(SKIA)/GrDrawTarget.obj $(OBJ_DIR)/$(SKIA)/GrFontScaler.obj\
			$(OBJ_DIR)/$(SKIA)/GrFrameBufferObj.obj $(OBJ_DIR)/$(SKIA)/GrGLAssembleInterface.obj $(OBJ_DIR)/$(SKIA)/GrGLBufferImpl.obj $(OBJ_DIR)/$(SKIA)/GrGLCaps.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLContext.obj $(OBJ_DIR)/$(SKIA)/GrGLCreateDebugInterface.obj $(OBJ_DIR)/$(SKIA)/GrGLCreateNativeInterface_win.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLCreateNullInterface.obj $(OBJ_DIR)/$(SKIA)/GrGLDefaultInterface_native.obj $(OBJ_DIR)/$(SKIA)/GrGLExtensions.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLFragmentOnlyProgramBuilder.obj $(OBJ_DIR)/$(SKIA)/GrGLFragmentShaderBuilder.obj $(OBJ_DIR)/$(SKIA)/GrGLFullProgramBuilder.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLGeometryShaderBuilder.obj $(OBJ_DIR)/$(SKIA)/GrGLIndexBuffer.obj $(OBJ_DIR)/$(SKIA)/GrGLInterface.obj $(OBJ_DIR)/$(SKIA)/GrGLNameAllocator.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLNoOpInterface.obj $(OBJ_DIR)/$(SKIA)/GrGLPath.obj $(OBJ_DIR)/$(SKIA)/GrGLPathRange.obj $(OBJ_DIR)/$(SKIA)/GrGLPathRendering.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLProgram.obj $(OBJ_DIR)/$(SKIA)/GrGLProgramBuilder.obj $(OBJ_DIR)/$(SKIA)/GrGLProgramDataManager.obj $(OBJ_DIR)/$(SKIA)/GrGLProgramDesc.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLProgramEffects.obj $(OBJ_DIR)/$(SKIA)/GrGLRenderTarget.obj $(OBJ_DIR)/$(SKIA)/GrGLSL.obj $(OBJ_DIR)/$(SKIA)/GrGLSLPrettyPrint.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLShaderBuilder.obj $(OBJ_DIR)/$(SKIA)/GrGLShaderStringBuilder.obj $(OBJ_DIR)/$(SKIA)/GrGLStencilBuffer.obj $(OBJ_DIR)/$(SKIA)/GrGLTexture.obj\
			$(OBJ_DIR)/$(SKIA)/GrGLUtil.obj $(OBJ_DIR)/$(SKIA)/GrGLVertexArray.obj $(OBJ_DIR)/$(SKIA)/GrGLVertexBuffer.obj $(OBJ_DIR)/$(SKIA)/GrGLVertexShaderBuilder.obj\
			$(OBJ_DIR)/$(SKIA)/GrGpu.obj $(OBJ_DIR)/$(SKIA)/GrGpuFactory.obj $(OBJ_DIR)/$(SKIA)/GrGpuGL.obj $(OBJ_DIR)/$(SKIA)/GrGpuGL_program.obj $(OBJ_DIR)/$(SKIA)/GrGpuResource.obj\
			$(OBJ_DIR)/$(SKIA)/GrGpuResourceRef.obj $(OBJ_DIR)/$(SKIA)/GrInOrderDrawBuffer.obj $(OBJ_DIR)/$(SKIA)/GrLayerCache.obj $(OBJ_DIR)/$(SKIA)/GrMatrixConvolutionEffect.obj\
			$(OBJ_DIR)/$(SKIA)/GrMemoryPool.obj $(OBJ_DIR)/$(SKIA)/GrOptDrawState.obj $(OBJ_DIR)/$(SKIA)/GrOvalEffect.obj $(OBJ_DIR)/$(SKIA)/GrOvalRenderer.obj\
			$(OBJ_DIR)/$(SKIA)/GrPaint.obj $(OBJ_DIR)/$(SKIA)/GrPath.obj $(OBJ_DIR)/$(SKIA)/GrPathRange.obj $(OBJ_DIR)/$(SKIA)/GrPathRenderer.obj\
			$(OBJ_DIR)/$(SKIA)/GrPathRendererChain.obj $(OBJ_DIR)/$(SKIA)/GrPathRendering.obj $(OBJ_DIR)/$(SKIA)/GrPathUtils.obj $(OBJ_DIR)/$(SKIA)/GrPictureUtils.obj\
			$(OBJ_DIR)/$(SKIA)/GrProcessor.obj $(OBJ_DIR)/$(SKIA)/GrProgramElement.obj $(OBJ_DIR)/$(SKIA)/GrProgramObj.obj $(OBJ_DIR)/$(SKIA)/GrRODrawState.obj\
			$(OBJ_DIR)/$(SKIA)/GrRRectEffect.obj $(OBJ_DIR)/$(SKIA)/GrRectanizer_pow2.obj $(OBJ_DIR)/$(SKIA)/GrRectanizer_skyline.obj $(OBJ_DIR)/$(SKIA)/GrReducedClip.obj\
			$(OBJ_DIR)/$(SKIA)/GrRenderTarget.obj $(OBJ_DIR)/$(SKIA)/GrResourceCache.obj $(OBJ_DIR)/$(SKIA)/GrResourceCache2.obj $(OBJ_DIR)/$(SKIA)/GrSWMaskHelper.obj\
			$(OBJ_DIR)/$(SKIA)/GrShaderObj.obj $(OBJ_DIR)/$(SKIA)/GrSimpleTextureEffect.obj $(OBJ_DIR)/$(SKIA)/GrSingleTextureEffect.obj $(OBJ_DIR)/$(SKIA)/GrSoftwarePathRenderer.obj\
			$(OBJ_DIR)/$(SKIA)/GrStencil.obj $(OBJ_DIR)/$(SKIA)/GrStencilAndCoverPathRenderer.obj $(OBJ_DIR)/$(SKIA)/GrStencilAndCoverTextContext.obj $(OBJ_DIR)/$(SKIA)/GrStencilBuffer.obj\
			$(OBJ_DIR)/$(SKIA)/GrSurface.obj $(OBJ_DIR)/$(SKIA)/GrTextContext.obj $(OBJ_DIR)/$(SKIA)/GrTextStrike.obj $(OBJ_DIR)/$(SKIA)/GrTexture.obj $(OBJ_DIR)/$(SKIA)/GrTextureAccess.obj\
			$(OBJ_DIR)/$(SKIA)/GrTextureDomain.obj $(OBJ_DIR)/$(SKIA)/GrTextureObj.obj $(OBJ_DIR)/$(SKIA)/GrTextureStripAtlas.obj $(OBJ_DIR)/$(SKIA)/GrTextureUnitObj.obj\
			$(OBJ_DIR)/$(SKIA)/GrTraceMarker.obj $(OBJ_DIR)/$(SKIA)/GrYUVtoRGBEffect.obj $(OBJ_DIR)/$(SKIA)/Sk1DPathEffect.obj $(OBJ_DIR)/$(SKIA)/Sk2DPathEffect.obj\
			$(OBJ_DIR)/$(SKIA)/SkAAClip.obj $(OBJ_DIR)/$(SKIA)/SkAddIntersections.obj $(OBJ_DIR)/$(SKIA)/SkAdvancedTypefaceMetrics.obj $(OBJ_DIR)/$(SKIA)/SkAlphaRuns.obj\
			$(OBJ_DIR)/$(SKIA)/SkAlphaThresholdFilter.obj $(OBJ_DIR)/$(SKIA)/SkAnnotation.obj $(OBJ_DIR)/$(SKIA)/SkArithmeticMode.obj $(OBJ_DIR)/$(SKIA)/SkAutoCoInitialize.obj\
			$(OBJ_DIR)/$(SKIA)/SkAvoidXfermode.obj $(OBJ_DIR)/$(SKIA)/SkBBHFactory.obj $(OBJ_DIR)/$(SKIA)/SkBBoxHierarchyRecord.obj $(OBJ_DIR)/$(SKIA)/SkBBoxRecord.obj\
			$(OBJ_DIR)/$(SKIA)/SkBase64.obj $(OBJ_DIR)/$(SKIA)/SkBitSet.obj $(OBJ_DIR)/$(SKIA)/SkBitmap.obj $(OBJ_DIR)/$(SKIA)/SkBitmapCache.obj $(OBJ_DIR)/$(SKIA)/SkBitmapDevice.obj\
			$(OBJ_DIR)/$(SKIA)/SkBitmapFilter.obj $(OBJ_DIR)/$(SKIA)/SkBitmapFilter_opts_SSE2.obj $(OBJ_DIR)/$(SKIA)/SkBitmapHasher.obj $(OBJ_DIR)/$(SKIA)/SkBitmapHeap.obj\
			$(OBJ_DIR)/$(SKIA)/SkBitmapProcShader.obj $(OBJ_DIR)/$(SKIA)/SkBitmapProcState.obj $(OBJ_DIR)/$(SKIA)/SkBitmapProcState_matrixProcs.obj\
			$(OBJ_DIR)/$(SKIA)/SkBitmapProcState_opts_SSE2.obj $(OBJ_DIR)/$(SKIA)/SkBitmapProcState_opts_SSSE3.obj $(OBJ_DIR)/$(SKIA)/SkBitmapScaler.obj\
			$(OBJ_DIR)/$(SKIA)/SkBitmapSource.obj $(OBJ_DIR)/$(SKIA)/SkBitmap_scroll.obj $(OBJ_DIR)/$(SKIA)/SkBlitMask_D32.obj $(OBJ_DIR)/$(SKIA)/SkBlitRect_opts_SSE2.obj\
			$(OBJ_DIR)/$(SKIA)/SkBlitRow_D16.obj $(OBJ_DIR)/$(SKIA)/SkBlitRow_D32.obj $(OBJ_DIR)/$(SKIA)/SkBlitRow_opts_SSE2.obj $(OBJ_DIR)/$(SKIA)/SkBlitter.obj\
			$(OBJ_DIR)/$(SKIA)/SkBlitter_A8.obj $(OBJ_DIR)/$(SKIA)/SkBlitter_ARGB32.obj $(OBJ_DIR)/$(SKIA)/SkBlitter_RGB16.obj $(OBJ_DIR)/$(SKIA)/SkBlitter_Sprite.obj\
			$(OBJ_DIR)/$(SKIA)/SkBlurDrawLooper.obj $(OBJ_DIR)/$(SKIA)/SkBlurImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkBlurImage_opts_SSE2.obj $(OBJ_DIR)/$(SKIA)/SkBlurImage_opts_SSE4.obj\
			$(OBJ_DIR)/$(SKIA)/SkBlurMask.obj $(OBJ_DIR)/$(SKIA)/SkBlurMaskFilter.obj $(OBJ_DIR)/$(SKIA)/SkBoundaryPatch.obj $(OBJ_DIR)/$(SKIA)/SkBuffer.obj\
			$(OBJ_DIR)/$(SKIA)/SkCachingPixelRef.obj $(OBJ_DIR)/$(SKIA)/SkCamera.obj $(OBJ_DIR)/$(SKIA)/SkCanvas.obj $(OBJ_DIR)/$(SKIA)/SkCanvasStack.obj\
			$(OBJ_DIR)/$(SKIA)/SkCanvasStateUtils.obj $(OBJ_DIR)/$(SKIA)/SkChunkAlloc.obj $(OBJ_DIR)/$(SKIA)/SkClampRange.obj $(OBJ_DIR)/$(SKIA)/SkClipStack.obj\
			$(OBJ_DIR)/$(SKIA)/SkColor.obj $(OBJ_DIR)/$(SKIA)/SkColorFilter.obj $(OBJ_DIR)/$(SKIA)/SkColorFilterImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkColorFilters.obj\
			$(OBJ_DIR)/$(SKIA)/SkColorMatrix.obj $(OBJ_DIR)/$(SKIA)/SkColorMatrixFilter.obj $(OBJ_DIR)/$(SKIA)/SkColorTable.obj $(OBJ_DIR)/$(SKIA)/SkComposeImageFilter.obj\
			$(OBJ_DIR)/$(SKIA)/SkComposeShader.obj $(OBJ_DIR)/$(SKIA)/SkCondVar.obj $(OBJ_DIR)/$(SKIA)/SkConfig8888.obj $(OBJ_DIR)/$(SKIA)/SkConvolver.obj\
			$(OBJ_DIR)/$(SKIA)/SkCornerPathEffect.obj $(OBJ_DIR)/$(SKIA)/SkCubicClipper.obj $(OBJ_DIR)/$(SKIA)/SkCubicInterval.obj $(OBJ_DIR)/$(SKIA)/SkCullPoints.obj\
			$(OBJ_DIR)/$(SKIA)/SkDCubicIntersection.obj $(OBJ_DIR)/$(SKIA)/SkDCubicLineIntersection.obj $(OBJ_DIR)/$(SKIA)/SkDCubicToQuads.obj $(OBJ_DIR)/$(SKIA)/SkDLineIntersection.obj\
			$(OBJ_DIR)/$(SKIA)/SkDQuadImplicit.obj $(OBJ_DIR)/$(SKIA)/SkDQuadIntersection.obj $(OBJ_DIR)/$(SKIA)/SkDQuadLineIntersection.obj $(OBJ_DIR)/$(SKIA)/SkDashPath.obj\
			$(OBJ_DIR)/$(SKIA)/SkDashPathEffect.obj $(OBJ_DIR)/$(SKIA)/SkData.obj $(OBJ_DIR)/$(SKIA)/SkDataTable.obj $(OBJ_DIR)/$(SKIA)/SkDebug.obj\
			$(OBJ_DIR)/$(SKIA)/SkDebugGLContext.obj $(OBJ_DIR)/$(SKIA)/SkDebug_win.obj $(OBJ_DIR)/$(SKIA)/SkDeque.obj $(OBJ_DIR)/$(SKIA)/SkDevice.obj\
			$(OBJ_DIR)/$(SKIA)/SkDeviceLooper.obj $(OBJ_DIR)/$(SKIA)/SkDeviceProfile.obj $(OBJ_DIR)/$(SKIA)/SkDiscardableMemoryPool.obj\
			$(OBJ_DIR)/$(SKIA)/SkDiscardableMemory_none.obj $(OBJ_DIR)/$(SKIA)/SkDiscardablePixelRef.obj $(OBJ_DIR)/$(SKIA)/SkDiscretePathEffect.obj\
			$(OBJ_DIR)/$(SKIA)/SkDisplacementMapEffect.obj $(OBJ_DIR)/$(SKIA)/SkDistanceFieldGen.obj $(OBJ_DIR)/$(SKIA)/SkDither.obj\
			$(OBJ_DIR)/$(SKIA)/SkDocument.obj $(OBJ_DIR)/$(SKIA)/SkDraw.obj $(OBJ_DIR)/$(SKIA)/SkDrawLooper.obj $(OBJ_DIR)/$(SKIA)/SkDropShadowImageFilter.obj\
			$(OBJ_DIR)/$(SKIA)/SkDumpCanvas.obj $(OBJ_DIR)/$(SKIA)/SkEdge.obj $(OBJ_DIR)/$(SKIA)/SkEdgeBuilder.obj $(OBJ_DIR)/$(SKIA)/SkEdgeClipper.obj\
			$(OBJ_DIR)/$(SKIA)/SkEmbossMask.obj $(OBJ_DIR)/$(SKIA)/SkEmbossMaskFilter.obj $(OBJ_DIR)/$(SKIA)/SkError.obj $(OBJ_DIR)/$(SKIA)/SkEventTracer.obj\
			$(OBJ_DIR)/$(SKIA)/SkFilterProc.obj $(OBJ_DIR)/$(SKIA)/SkFilterShader.obj $(OBJ_DIR)/$(SKIA)/SkFlattenable.obj $(OBJ_DIR)/$(SKIA)/SkFlattenableSerialization.obj\
			$(OBJ_DIR)/$(SKIA)/SkFloat.obj $(OBJ_DIR)/$(SKIA)/SkFloatBits.obj $(OBJ_DIR)/$(SKIA)/SkFont.obj $(OBJ_DIR)/$(SKIA)/SkFontDescriptor.obj $(OBJ_DIR)/$(SKIA)/SkFontHost.obj\
			$(OBJ_DIR)/$(SKIA)/SkFontHost_win.obj $(OBJ_DIR)/$(SKIA)/SkFontMgr_default_gdi.obj $(OBJ_DIR)/$(SKIA)/SkFontMgr_indirect.obj $(OBJ_DIR)/$(SKIA)/SkFontStream.obj\
			$(OBJ_DIR)/$(SKIA)/SkFrontBufferedStream.obj $(OBJ_DIR)/$(SKIA)/SkGLContextHelper.obj $(OBJ_DIR)/$(SKIA)/SkGScalerContext.obj\
			$(OBJ_DIR)/$(SKIA)/SkGatherPixelRefsAndRects.obj $(OBJ_DIR)/$(SKIA)/SkGeometry.obj $(OBJ_DIR)/$(SKIA)/SkGlobalInitialization_default.obj $(OBJ_DIR)/$(SKIA)/SkGlyphCache.obj\
			$(OBJ_DIR)/$(SKIA)/SkGpuBlurUtils.obj $(OBJ_DIR)/$(SKIA)/SkGpuDevice.obj $(OBJ_DIR)/$(SKIA)/SkGr.obj $(OBJ_DIR)/$(SKIA)/SkGrPixelRef.obj\
			$(OBJ_DIR)/$(SKIA)/SkGrTexturePixelRef.obj $(OBJ_DIR)/$(SKIA)/SkGradientBitmapCache.obj $(OBJ_DIR)/$(SKIA)/SkGradientShader.obj $(OBJ_DIR)/$(SKIA)/SkGraphics.obj\
			$(OBJ_DIR)/$(SKIA)/SkHRESULT.obj $(OBJ_DIR)/$(SKIA)/SkIStream.obj $(OBJ_DIR)/$(SKIA)/SkImage.obj $(OBJ_DIR)/$(SKIA)/SkImageDecoder.obj\
			$(OBJ_DIR)/$(SKIA)/SkImageEncoder.obj $(OBJ_DIR)/$(SKIA)/SkImageEncoder_Factory.obj $(OBJ_DIR)/$(SKIA)/SkImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkImageGenerator.obj\
			$(OBJ_DIR)/$(SKIA)/SkImageInfo.obj $(OBJ_DIR)/$(SKIA)/SkImagePriv.obj $(OBJ_DIR)/$(SKIA)/SkImage_Raster.obj $(OBJ_DIR)/$(SKIA)/SkInstCnt.obj\
			$(OBJ_DIR)/$(SKIA)/SkInterpolator.obj $(OBJ_DIR)/$(SKIA)/SkIntersections.obj $(OBJ_DIR)/$(SKIA)/SkLayer.obj $(OBJ_DIR)/$(SKIA)/SkLayerDrawLooper.obj\
			$(OBJ_DIR)/$(SKIA)/SkLayerRasterizer.obj $(OBJ_DIR)/$(SKIA)/SkLerpXfermode.obj $(OBJ_DIR)/$(SKIA)/SkLightingImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkLineClipper.obj\
			$(OBJ_DIR)/$(SKIA)/SkLinearGradient.obj $(OBJ_DIR)/$(SKIA)/SkLocalMatrixShader.obj $(OBJ_DIR)/$(SKIA)/SkLumaColorFilter.obj $(OBJ_DIR)/$(SKIA)/SkMD5.obj\
			$(OBJ_DIR)/$(SKIA)/SkMagnifierImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkMallocPixelRef.obj $(OBJ_DIR)/$(SKIA)/SkMask.obj $(OBJ_DIR)/$(SKIA)/SkMaskFilter.obj\
			$(OBJ_DIR)/$(SKIA)/SkMaskGamma.obj $(OBJ_DIR)/$(SKIA)/SkMath.obj $(OBJ_DIR)/$(SKIA)/SkMatrix.obj $(OBJ_DIR)/$(SKIA)/SkMatrix22.obj $(OBJ_DIR)/$(SKIA)/SkMatrix44.obj\
			$(OBJ_DIR)/$(SKIA)/SkMatrixConvolutionImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkMatrixImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkMemory_malloc.obj\
			$(OBJ_DIR)/$(SKIA)/SkMergeImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkMeshUtils.obj $(OBJ_DIR)/$(SKIA)/SkMetaData.obj $(OBJ_DIR)/$(SKIA)/SkMipMap.obj\
			$(OBJ_DIR)/$(SKIA)/SkMorphologyImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkMorphology_opts_SSE2.obj $(OBJ_DIR)/$(SKIA)/SkMultiPictureDraw.obj $(OBJ_DIR)/$(SKIA)/SkNWayCanvas.obj\
			$(OBJ_DIR)/$(SKIA)/SkNativeGLContext_win.obj $(OBJ_DIR)/$(SKIA)/SkNinePatch.obj $(OBJ_DIR)/$(SKIA)/SkNullCanvas.obj $(OBJ_DIR)/$(SKIA)/SkNullGLContext.obj\
			$(OBJ_DIR)/$(SKIA)/SkOSFile.obj $(OBJ_DIR)/$(SKIA)/SkOSFile_stdio.obj $(OBJ_DIR)/$(SKIA)/SkOSFile_win.obj $(OBJ_DIR)/$(SKIA)/SkOTTable_name.obj\
			$(OBJ_DIR)/$(SKIA)/SkOTUtils.obj $(OBJ_DIR)/$(SKIA)/SkOffsetImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkOpAngle.obj $(OBJ_DIR)/$(SKIA)/SkOpContour.obj\
			$(OBJ_DIR)/$(SKIA)/SkOpEdgeBuilder.obj $(OBJ_DIR)/$(SKIA)/SkOpSegment.obj $(OBJ_DIR)/$(SKIA)/SkPackBits.obj $(OBJ_DIR)/$(SKIA)/SkPaint.obj\
			$(OBJ_DIR)/$(SKIA)/SkPaintFlagsDrawFilter.obj $(OBJ_DIR)/$(SKIA)/SkPaintPriv.obj $(OBJ_DIR)/$(SKIA)/SkParse.obj $(OBJ_DIR)/$(SKIA)/SkParseColor.obj\
			$(OBJ_DIR)/$(SKIA)/SkParsePath.obj $(OBJ_DIR)/$(SKIA)/SkPatchGrid.obj $(OBJ_DIR)/$(SKIA)/SkPatchUtils.obj $(OBJ_DIR)/$(SKIA)/SkPath.obj $(OBJ_DIR)/$(SKIA)/SkPathEffect.obj\
			$(OBJ_DIR)/$(SKIA)/SkPathHeap.obj $(OBJ_DIR)/$(SKIA)/SkPathMeasure.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsBounds.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsCommon.obj\
			$(OBJ_DIR)/$(SKIA)/SkPathOpsCubic.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsDebug.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsLine.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsOp.obj\
			$(OBJ_DIR)/$(SKIA)/SkPathOpsPoint.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsQuad.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsRect.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsSimplify.obj\
			$(OBJ_DIR)/$(SKIA)/SkPathOpsTightBounds.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsTriangle.obj $(OBJ_DIR)/$(SKIA)/SkPathOpsTypes.obj $(OBJ_DIR)/$(SKIA)/SkPathRef.obj\
			$(OBJ_DIR)/$(SKIA)/SkPathUtils.obj $(OBJ_DIR)/$(SKIA)/SkPathWriter.obj $(OBJ_DIR)/$(SKIA)/SkPerlinNoiseShader.obj $(OBJ_DIR)/$(SKIA)/SkPicture.obj\
			$(OBJ_DIR)/$(SKIA)/SkPictureContentInfo.obj $(OBJ_DIR)/$(SKIA)/SkPictureData.obj $(OBJ_DIR)/$(SKIA)/SkPictureFlat.obj $(OBJ_DIR)/$(SKIA)/SkPictureImageFilter.obj\
			$(OBJ_DIR)/$(SKIA)/SkPicturePlayback.obj $(OBJ_DIR)/$(SKIA)/SkPictureRecord.obj $(OBJ_DIR)/$(SKIA)/SkPictureRecorder.obj $(OBJ_DIR)/$(SKIA)/SkPictureShader.obj\
			$(OBJ_DIR)/$(SKIA)/SkPictureStateTree.obj $(OBJ_DIR)/$(SKIA)/SkPictureUtils.obj $(OBJ_DIR)/$(SKIA)/SkPixelRef.obj $(OBJ_DIR)/$(SKIA)/SkPixelXorXfermode.obj\
			$(OBJ_DIR)/$(SKIA)/SkPoint.obj $(OBJ_DIR)/$(SKIA)/SkPorterDuff.obj $(OBJ_DIR)/$(SKIA)/SkProcSpriteBlitter.obj $(OBJ_DIR)/$(SKIA)/SkProxyCanvas.obj\
			$(OBJ_DIR)/$(SKIA)/SkPtrRecorder.obj $(OBJ_DIR)/$(SKIA)/SkQuadClipper.obj $(OBJ_DIR)/$(SKIA)/SkQuarticRoot.obj $(OBJ_DIR)/$(SKIA)/SkRRect.obj\
			$(OBJ_DIR)/$(SKIA)/SkRTConf.obj $(OBJ_DIR)/$(SKIA)/SkRTree.obj $(OBJ_DIR)/$(SKIA)/SkRadialGradient.obj $(OBJ_DIR)/$(SKIA)/SkRasterClip.obj $(OBJ_DIR)/$(SKIA)/SkRasterizer.obj\
			$(OBJ_DIR)/$(SKIA)/SkReadBuffer.obj $(OBJ_DIR)/$(SKIA)/SkRecordDraw.obj $(OBJ_DIR)/$(SKIA)/SkRecordOpts.obj $(OBJ_DIR)/$(SKIA)/SkRecorder.obj\
			$(OBJ_DIR)/$(SKIA)/SkRecording.obj $(OBJ_DIR)/$(SKIA)/SkRect.obj $(OBJ_DIR)/$(SKIA)/SkRectShaderImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkReduceOrder.obj\
			$(OBJ_DIR)/$(SKIA)/SkRefDict.obj $(OBJ_DIR)/$(SKIA)/SkRegion.obj $(OBJ_DIR)/$(SKIA)/SkRegion_path.obj $(OBJ_DIR)/$(SKIA)/SkRemotableFontMgr.obj\
			$(OBJ_DIR)/$(SKIA)/SkResourceCache.obj $(OBJ_DIR)/$(SKIA)/SkSHA1.obj $(OBJ_DIR)/$(SKIA)/SkScalar.obj $(OBJ_DIR)/$(SKIA)/SkScalerContext.obj\
			$(OBJ_DIR)/$(SKIA)/SkScan.obj $(OBJ_DIR)/$(SKIA)/SkScan_AntiPath.obj $(OBJ_DIR)/$(SKIA)/SkScan_Antihair.obj $(OBJ_DIR)/$(SKIA)/SkScan_Hairline.obj\
			$(OBJ_DIR)/$(SKIA)/SkScan_Path.obj $(OBJ_DIR)/$(SKIA)/SkShader.obj $(OBJ_DIR)/$(SKIA)/SkSpriteBlitter_ARGB32.obj $(OBJ_DIR)/$(SKIA)/SkSpriteBlitter_RGB16.obj\
			$(OBJ_DIR)/$(SKIA)/SkStream.obj $(OBJ_DIR)/$(SKIA)/SkString.obj $(OBJ_DIR)/$(SKIA)/SkStringUtils.obj $(OBJ_DIR)/$(SKIA)/SkStroke.obj $(OBJ_DIR)/$(SKIA)/SkStrokeRec.obj\
			$(OBJ_DIR)/$(SKIA)/SkStrokerPriv.obj $(OBJ_DIR)/$(SKIA)/SkSurface.obj $(OBJ_DIR)/$(SKIA)/SkSurface_Raster.obj $(OBJ_DIR)/$(SKIA)/SkSweepGradient.obj\
			$(OBJ_DIR)/$(SKIA)/SkTLS.obj $(OBJ_DIR)/$(SKIA)/SkTLS_win.obj $(OBJ_DIR)/$(SKIA)/SkTSearch.obj $(OBJ_DIR)/$(SKIA)/SkTableColorFilter.obj\
			$(OBJ_DIR)/$(SKIA)/SkTableMaskFilter.obj $(OBJ_DIR)/$(SKIA)/SkTestImageFilters.obj $(OBJ_DIR)/$(SKIA)/SkTestScalerContext.obj\
			$(OBJ_DIR)/$(SKIA)/SkTextBlob.obj $(OBJ_DIR)/$(SKIA)/SkTextureCompression_opts_none.obj $(OBJ_DIR)/$(SKIA)/SkTextureCompressor.obj\
			$(OBJ_DIR)/$(SKIA)/SkTextureCompressor_ASTC.obj $(OBJ_DIR)/$(SKIA)/SkTextureCompressor_LATC.obj $(OBJ_DIR)/$(SKIA)/SkTextureCompressor_R11EAC.obj\
			$(OBJ_DIR)/$(SKIA)/SkThreadUtils_win.obj $(OBJ_DIR)/$(SKIA)/SkTileGrid.obj $(OBJ_DIR)/$(SKIA)/SkTileImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkTime_win.obj\
			$(OBJ_DIR)/$(SKIA)/SkTransparentShader.obj $(OBJ_DIR)/$(SKIA)/SkTwoPointConicalGradient.obj $(OBJ_DIR)/$(SKIA)/SkTwoPointConicalGradient_gpu.obj\
			$(OBJ_DIR)/$(SKIA)/SkTwoPointRadialGradient.obj $(OBJ_DIR)/$(SKIA)/SkTypeface.obj $(OBJ_DIR)/$(SKIA)/SkTypefaceCache.obj $(OBJ_DIR)/$(SKIA)/SkUnPreMultiply.obj\
			$(OBJ_DIR)/$(SKIA)/SkUtils.obj $(OBJ_DIR)/$(SKIA)/SkUtils_opts_SSE2.obj $(OBJ_DIR)/$(SKIA)/SkValidatingReadBuffer.obj $(OBJ_DIR)/$(SKIA)/SkVertState.obj\
			$(OBJ_DIR)/$(SKIA)/SkWGL_win.obj $(OBJ_DIR)/$(SKIA)/SkWriteBuffer.obj $(OBJ_DIR)/$(SKIA)/SkWriter32.obj $(OBJ_DIR)/$(SKIA)/SkXfermode.obj\
			$(OBJ_DIR)/$(SKIA)/SkXfermodeImageFilter.obj $(OBJ_DIR)/$(SKIA)/SkXfermode_opts_SSE2.obj $(OBJ_DIR)/$(SKIA)/opts_check_x86.obj

LOG4Z = log4z
LOG4Z_DIR = components/$(LOG4Z)
LOG4Z_OBJS = $(OBJ_DIR)/$(LOG4Z)/log4z.obj

RES_7Z = resprovider-7zip
RES_7Z_DIR = components/$(RES_7Z)
RES_7Z_FLAGS = -Ithird-part/$(7Z) -I$(SOUI)/include -I$(UTILITIES)/include
RES_7Z_OBJS = $(OBJ_DIR)/$(RES_7Z)/SResProvider7Zip.obj $(OBJ_DIR)/$(RES_7Z)/cursoricon.obj $(OBJ_DIR)/$(RES_7Z)/zip7Archive.obj

RES_ZIP = resprovider-zip
RES_ZIP_DIR = components/$(RES_ZIP)
RES_ZIP_FLAGS = -Ithird-part/$(ZLIB) -I$(SOUI)/include -I$(UTILITIES)/include
RES_ZIP_OBJS = $(OBJ_DIR)/$(RES_ZIP)/SResProviderZip.obj $(OBJ_DIR)/$(RES_ZIP)/cursoricon.obj $(OBJ_DIR)/$(RES_ZIP)/zipArchive.obj

TRANSLATOR = translator
TRANSLATOR_DIR = components/$(TRANSLATOR)
TRANSLATOR_FLAGS = -I$(SOUI)/include -I$(UTILITIES)/include
TRANSLATOR_OBJS = $(OBJ_DIR)/$(TRANSLATOR)/translator.obj

SCRIPTMODULE = scriptmodule-lua
SCRIPTMODULE_DIR = components/$(SCRIPTMODULE)
SCRIPTMODULE_SRC = $(SCRIPTMODULE_DIR)/src
SCRIPTMODULE_FLAGS = -I$(SOUI)/include -I$(UTILITIES)/include -I$(SCRIPTMODULE_SRC) -I$(LUA_SRC)
SCRIPTMODULE_OBJS = $(OBJ_DIR)/$(SCRIPTMODULE)/ScriptModule-Lua.obj $(OBJ_DIR)/$(SCRIPTMODULE)/exp_soui.obj $(OBJ_DIR)/$(SCRIPTMODULE)/lua_tinker.obj

RENDER_GDI = render-gdi
RENDER_GDI_DIR = components/$(RENDER_GDI)
RENDER_GDI_FLAGS = -I$(SOUI)/include -I$(UTILITIES)/include
RENDER_GDI_OBJS = $(OBJ_DIR)/$(RENDER_GDI)/GradientFillHelper.obj $(OBJ_DIR)/$(RENDER_GDI)/render-gdi.obj

RENDER_SKIA = render-skia
RENDER_SKIA_DIR = components/$(RENDER_SKIA)
RENDER_SKIA_FLAGS = -I$(SOUI)/include -I$(UTILITIES)/include -Ithird-part/$(SKIA) -Ithird-part/$(SKIA)/include -Ithird-part/$(SKIA)/include/config -Ithird-part/$(SKIA)/include/core
RENDER_SKIA_OBJS = $(OBJ_DIR)/$(RENDER_SKIA)/drawtext-skia.obj $(OBJ_DIR)/$(RENDER_SKIA)/PathEffect-Skia.obj $(OBJ_DIR)/$(RENDER_SKIA)/PathMeasure-Skia.obj\
					$(OBJ_DIR)/$(RENDER_SKIA)/render-skia.obj $(OBJ_DIR)/$(RENDER_SKIA)/render-skia2.obj $(OBJ_DIR)/$(RENDER_SKIA)/skia2rop2.obj

IMG_DECODER_PNG = imgdecoder-png
IMG_DECODER_PNG_DIR = components/$(IMG_DECODER_PNG)
IMG_DECODER_PNG_FLAGS = -Ithird-part/$(PNG) -Ithird-part/$(ZLIB) -I$(SOUI)/include -I$(UTILITIES)/include
IMG_DECODER_PNG_OBJS = $(OBJ_DIR)/$(IMG_DECODER_PNG)/decoder-apng.obj $(OBJ_DIR)/$(IMG_DECODER_PNG)/imgdecoder-png.obj

IMG_DECODER_GDIP = imgdecoder-gdip
IMG_DECODER_GDIP_DIR = components/$(IMG_DECODER_GDIP)
IMG_DECODER_GDIP_FLAGS = -I$(SOUI)/include -I$(UTILITIES)/include
IMG_DECODER_GDIP_OBJS = $(OBJ_DIR)/$(IMG_DECODER_GDIP)/imgdecoder-gdip.obj

IMG_DECODER_STB = imgdecoder-stb
IMG_DECODER_STB_DIR = components/$(IMG_DECODER_STB)
IMG_DECODER_STB_FLAGS = -I$(SOUI)/include -I$(UTILITIES)/include
IMG_DECODER_STB_OBJS = $(OBJ_DIR)/$(IMG_DECODER_STB)/imgdecoder-stb.obj

IMG_DECODER_WIC = imgdecoder-wic
IMG_DECODER_WIC_DIR = components/$(IMG_DECODER_WIC)
IMG_DECODER_WIC_FLAGS = -I$(SOUI)/include -I$(UTILITIES)/include
IMG_DECODER_WIC_OBJS = $(OBJ_DIR)/$(IMG_DECODER_WIC)/imgdecoder-wic.obj

DEMO = demo
DEMO_DIR = $(DEMO)
DEMO_FLAGS = -I$(DEMO_DIR) -I$(UTILITIES)/include -I$(SOUI)/include -Ithird-part/wke/include -Ithird-part/$(MHOOK)/mhook-lib -Icomponents
DEMO_OBJS = $(OBJ_DIR)/$(DEMO)/FormatMsgDlg.obj $(OBJ_DIR)/$(DEMO)/MainDlg.obj $(OBJ_DIR)/$(DEMO)/QR_Encode.obj $(OBJ_DIR)/$(DEMO)/SChromeTabCtrl.obj\
			$(OBJ_DIR)/$(DEMO)/SDemoSkin.obj $(OBJ_DIR)/$(DEMO)/SDocHostUIHandler.obj $(OBJ_DIR)/$(DEMO)/SFreeMoveWindow.obj $(OBJ_DIR)/$(DEMO)/SGifPlayer.obj\
			$(OBJ_DIR)/$(DEMO)/SHeaderCtrlEx.obj $(OBJ_DIR)/$(DEMO)/SInterpolatorView.obj $(OBJ_DIR)/$(DEMO)/SMCListViewEx.obj $(OBJ_DIR)/$(DEMO)/SPathView.obj\
			$(OBJ_DIR)/$(DEMO)/SQrCtrl.obj $(OBJ_DIR)/$(DEMO)/SRatingBar.obj $(OBJ_DIR)/$(DEMO)/SShellNofityHwnd2.obj $(OBJ_DIR)/$(DEMO)/SShellNotifyIcon.obj\
			$(OBJ_DIR)/$(DEMO)/SSkinAPNG.obj $(OBJ_DIR)/$(DEMO)/SSkinGif.obj $(OBJ_DIR)/$(DEMO)/SSkinImgFrame3.obj $(OBJ_DIR)/$(DEMO)/SSkinLoader.obj\
			$(OBJ_DIR)/$(DEMO)/STabCtrlHeaderBinder.obj $(OBJ_DIR)/$(DEMO)/SWkeWebkit.obj $(OBJ_DIR)/$(DEMO)/SetSkinWnd2.obj $(OBJ_DIR)/$(DEMO)/SmileyCreateHook.obj\
			$(OBJ_DIR)/$(DEMO)/dataobject.obj $(OBJ_DIR)/$(DEMO)/demo.obj $(OBJ_DIR)/$(DEMO)/genericserver.obj $(OBJ_DIR)/$(DEMO)/httpserver.obj\
			$(OBJ_DIR)/$(DEMO)/magnetframe.obj $(OBJ_DIR)/$(DEMO)/memflash.obj $(OBJ_DIR)/$(DEMO)/richeditole.obj $(OBJ_DIR)/$(DEMO)/scalendar2.obj\
			$(OBJ_DIR)/$(DEMO)/schatedit.obj $(OBJ_DIR)/$(DEMO)/sclock.obj $(OBJ_DIR)/$(DEMO)/sdesktopdock.obj $(OBJ_DIR)/$(DEMO)/sfadeframe.obj\
			$(OBJ_DIR)/$(DEMO)/siectrl.obj $(OBJ_DIR)/$(DEMO)/simagemaskwnd.obj $(OBJ_DIR)/$(DEMO)/sipaddressctrl.obj $(OBJ_DIR)/$(DEMO)/slistctrlex.obj\
			$(OBJ_DIR)/$(DEMO)/smatrixwindow.obj $(OBJ_DIR)/$(DEMO)/spropertygrid.obj $(OBJ_DIR)/$(DEMO)/spropertyitem-color.obj $(OBJ_DIR)/$(DEMO)/spropertyitem-option.obj\
			$(OBJ_DIR)/$(DEMO)/spropertyitem-size.obj $(OBJ_DIR)/$(DEMO)/spropertyitem-text.obj $(OBJ_DIR)/$(DEMO)/spropertyitembase.obj $(OBJ_DIR)/$(DEMO)/sradiobox2.obj\
			$(OBJ_DIR)/$(DEMO)/sscrolltext.obj $(OBJ_DIR)/$(DEMO)/threadObject.obj $(OBJ_DIR)/$(DEMO)/tipwnd.obj $(OBJ_DIR)/$(DEMO)/uianimationwnd.obj\
			$(OBJ_DIR)/$(DEMO)/stdafx.h.obj $(OBJ_DIR)/$(DEMO)/demo.res


#默认只编译gdip的图片解码器
#7z 默认不使用,要使用7z自行编译,例如"nmake resprovider-7zip"
#gtest与jsoncpp有需要自行编译,例如"nmake gtest"
all: precheck $(MHOOK) $(SMILEY) $(SOUI_RESOURCE) $(UTILITIES) $(SOUI) $(SCRIPTMODULE) $(LOG4Z) $(RES_ZIP) $(TRANSLATOR) $(RENDER_GDI) $(RENDER_SKIA) $(IMG_DECODER_PNG) $(DEMO)
#=======================================================================utilities=======================================================================
{$(UTILITIES_SRC)/pugixml}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(UTILITIES_FLAGS) -DUTILITIES_EXPORTS -Fo$(OBJ_DIR)/$(UTILITIES)/ -Fd$(OUTPUT_DIR)/$(UTILITIES)$(EXT) -c $<

{$(UTILITIES_SRC)/sobject}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(UTILITIES_FLAGS) -DUTILITIES_EXPORTS -Fo$(OBJ_DIR)/$(UTILITIES)/ -Fd$(OUTPUT_DIR)/$(UTILITIES)$(EXT) -c $<

{$(UTILITIES_SRC)/string}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(UTILITIES_FLAGS) -DUTILITIES_EXPORTS -Fo$(OBJ_DIR)/$(UTILITIES)/ -Fd$(OUTPUT_DIR)/$(UTILITIES)$(EXT) -c $<

{$(UTILITIES_SRC)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(UTILITIES_FLAGS) -DUTILITIES_EXPORTS -Fo$(OBJ_DIR)/$(UTILITIES)/ -Fd$(OUTPUT_DIR)/$(UTILITIES)$(EXT) -c $<

$(UTILITIES): precheck $(UTILITIES_OBJS)
	$(LD) $(LDFLAGS) /OUT:"$(OUTPUT_DIR)/$(UTILITIES)$(EXT).dll" /DLL $(UTILITIES_OBJS) $(LIBS)

#=======================================================================soui=======================================================================
{$(SOUI_SRC)/activex}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

{$(SOUI_SRC)/animator}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

{$(SOUI_SRC)/control}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

{$(SOUI_SRC)/core}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<
	
{$(SOUI_SRC)/event}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

{$(SOUI_SRC)/helper}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

{$(SOUI_SRC)/layout}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

{$(SOUI_SRC)/res.mgr}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

{$(SOUI_SRC)/updatelayeredwindow}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

{$(SOUI_SRC)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SOUI_FLAGS) -Fo$(OBJ_DIR)/$(SOUI)/ -Fd$(OUTPUT_DIR)/$(SOUI)$(EXT) -c $<

$(SOUI): precheck presoui $(SOUI_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(SOUI)$(EXT).dll" -dll $(SOUI_OBJS) $(LIBS) $(UTILITIES)$(EXT).lib

#=======================================================================soui-sys-resource=======================================================================

$(OBJ_DIR)/$(SOUI_RESOURCE)/$(SOUI_RESOURCE).res: $(SOUI_RESOURCE_DIR)/$(SOUI_RESOURCE).rc
	$(RC) -I$(SOUI_RESOURCE_DIR) -Fo"$(OBJ_DIR)/$(SOUI_RESOURCE)/$(SOUI_RESOURCE).res" $(SOUI_RESOURCE_DIR)/$(SOUI_RESOURCE).rc

$(SOUI_RESOURCE): precheck $(SOUI_RESOURCE_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(SOUI_RESOURCE)$(EXT).dll" -dll -noentry $(SOUI_RESOURCE_OBJS) $(LIBS)

#=======================================================================zlib=======================================================================

{$(ZLIB_DIR)}.c.obj:
	$(CC) $(CFLAGS) -Fo$(OBJ_DIR)/$(ZLIB)/ -Fd$(OUTPUT_DIR)/$(ZLIB)$(EXT) -c $<

$(ZLIB): precheck $(ZLIB_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(ZLIB)$(EXT).lib" $(ZLIB_OBJS)

#=======================================================================png=======================================================================
{$(PNG_DIR)}.c.obj:
	$(CC) $(CFLAGS) -I$(ZLIB_DIR) -Fo$(OBJ_DIR)/$(PNG)/ -Fd$(OUTPUT_DIR)/$(PNG)$(EXT) -c $<

$(PNG): precheck $(PNG_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(PNG)$(EXT).lib" $(PNG_OBJS)

#=======================================================================smiley=======================================================================
{$(SMILEY_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) -Fo$(OBJ_DIR)/$(SMILEY)/ -Fd$(OUTPUT_DIR)/$(SMILEY)$(EXT) -c $<

$(SMILEY): precheck $(SMILEY_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(SMILEY)$(EXT).lib" $(SMILEY_OBJS)

#=======================================================================mhook=======================================================================
{$(MHOOK_DIR)/disasm-lib}.c.obj:
	$(CC) $(CFLAGS) -Fo$(OBJ_DIR)/$(MHOOK)/ -Fd$(OUTPUT_DIR)/$(MHOOK)$(EXT) -c $<

{$(MHOOK_DIR)/mhook-lib}.cpp.obj:
	$(CXX) $(CXXFLAGS) -Fo$(OBJ_DIR)/$(MHOOK)/ -Fd$(OUTPUT_DIR)/$(MHOOK)$(EXT) -c $<

$(MHOOK): precheck $(MHOOK_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(MHOOK)$(EXT).lib" $(MHOOK_OBJS)

#=======================================================================gtest=======================================================================
$(OBJ_DIR)/$(GTEST)/gtest-all.obj: $(GTEST_SRC)/gtest-all.cc
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR) -I$(GTEST_DIR)/include -Fo$(OBJ_DIR)/$(GTEST)/gtest-all.obj -Fd$(OUTPUT_DIR)/$(GTEST)$(EXT) -c $(GTEST_SRC)/gtest-all.cc

$(OBJ_DIR)/$(GTEST)/gtest_main.obj: $(GTEST_SRC)/gtest_main.cc
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR) -I$(GTEST_DIR)/include -Fo$(OBJ_DIR)/$(GTEST)/gtest_main.obj -Fd$(OUTPUT_DIR)/$(GTEST)$(EXT) -c $(GTEST_SRC)/gtest_main.cc

$(GTEST): precheck $(GTEST_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(GTEST)$(EXT).lib" $(GTEST_OBJS)

#=======================================================================lua=======================================================================
{$(LUA_SRC)}.c.obj:
	$(CC) $(CFLAGS) -Fo$(OBJ_DIR)/$(LUA)/ -Fd$(OUTPUT_DIR)/$(LUA)$(EXT) -c $<

$(LUA): precheck $(LUA_LIB_OBJS) $(LUA_OBJS) $(LUA_COMPILER_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(LUA)$(EXT).lib" $(LUA_LIB_OBJS)
	$(LD) $(LDCONSOLEFLAGS) -out:"$(OUTPUT_DIR)/$(LUA)$(EXT).exe" $(LUA_OBJS) $(LIBS) $(LUA)$(EXT).lib
	$(LD) $(LDCONSOLEFLAGS) -out:"$(OUTPUT_DIR)/$(LUA)c$(EXT).exe" $(LUA_COMPILER_OBJS) $(LIBS) $(LUA)$(EXT).lib

#=======================================================================jsoncpp=======================================================================
{$(JSONCPP_SRC)/lib_json}.cpp.obj:
	$(CXX) $(CXXFLAGS) -I$(JSONCPP_DIR)/include -Fo$(OBJ_DIR)/$(JSONCPP)/ -Fd$(OUTPUT_DIR)/$(JSONCPP)$(EXT) -c $<

$(JSONCPP): precheck $(JSONCPP_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(JSONCPP)$(EXT).lib" $(JSONCPP_OBJS)

#=======================================================================scintilla=======================================================================
{$(SCINTILLA_DIR)/src}.cxx.obj:
	$(CXX) $(CXXFLAGS) $(SCINTILLA_HEADER) -Fo$(OBJ_DIR)/$(SCINTILLA)/ -Fd$(OUTPUT_DIR)/$(SCINTILLA)$(EXT) -c $<

{$(SCINTILLA_DIR)/win32}.cxx.obj:
	$(CXX) $(CXXFLAGS) $(SCINTILLA_HEADER) -Fo$(OBJ_DIR)/$(SCINTILLA)/ -Fd$(OUTPUT_DIR)/$(SCINTILLA)$(EXT) -c $<

{$(SCINTILLA_DIR)/lexers}.cxx.obj:
	$(CXX) $(CXXFLAGS) $(SCINTILLA_HEADER) -Fo$(OBJ_DIR)/$(SCINTILLA)/ -Fd$(OUTPUT_DIR)/$(SCINTILLA)$(EXT) -c $<

{$(SCINTILLA_DIR)/lexlib}.cxx.obj:
	$(CXX) $(CXXFLAGS) $(SCINTILLA_HEADER) -Fo$(OBJ_DIR)/$(SCINTILLA)/ -Fd$(OUTPUT_DIR)/$(SCINTILLA)$(EXT) -c $<

$(SCINTILLA): precheck $(SCINTILLA_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(SCINTILLA)$(EXT).lib" $(SCINTILLA_OBJS)

#=======================================================================7z=======================================================================
$(OBJ_DIR)/$(7Z)/AesOpt.obj: $(7Z_DIR)/Asm/x86/AesOpt.asm
	$(ML) -c -WX -W3 -Fo$(OBJ_DIR)/$(7Z)/AesOpt.obj $(7Z_DIR)/Asm/x86/AesOpt.asm

$(OBJ_DIR)/$(7Z)/7zCrcOpt.obj: $(7Z_DIR)/Asm/x86/7zCrcOpt.asm
	$(ML) -c -WX -W3 -Fo$(OBJ_DIR)/$(7Z)/7zCrcOpt.obj $(7Z_DIR)/Asm/x86/7zCrcOpt.asm

{$(7Z_DIR)/CPP/7zip/Crypto}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Compress}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/7z}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/SevenZip}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/Common}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Common}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Cab}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Chm}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Common}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/Windows}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Iso}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Nsis}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Tar}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Udf}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Wim}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/CPP/7zip/Archive/Zip}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

{$(7Z_DIR)/C}.c.obj:
	$(CC) $(CFLAGS) $(7Z_FLAGS) -Fo$(OBJ_DIR)/$(7Z)/ -Fd$(OUTPUT_DIR)/$(7Z)$(EXT) -c $<

$(7Z): precheck $(7Z_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(7Z)$(EXT).lib" $(7Z_OBJS)

#=======================================================================skia=======================================================================

{$(SKIA_SRC)/gpu}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/gpu/effects}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/gpu/gl}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/gpu/gl/builders}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/gpu/gl/debug}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/gpu/gl/win}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/effects}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/core}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/pathops}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/utils/win}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/utils}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/opts}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/lazy}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/effects}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/effects/gradients}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/ports}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/doc}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/fonts}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/image}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/images}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

{$(SKIA_SRC)/sfnt}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SKIA_FLAGS) -Fo$(OBJ_DIR)/$(SKIA)/ -Fd$(OUTPUT_DIR)/$(SKIA)$(EXT) -c $<

$(SKIA): precheck $(SKIA_OBJS)
	$(LIB_EXE) -nologo $(MACHINE) -out:"$(OUTPUT_DIR)/$(SKIA)$(EXT).lib" $(SKIA_OBJS)

#=======================================================================skia=======================================================================

{$(LOG4Z_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) -I$(UTILITIES_DIR)/include -I$(SOUI_DIR)/include -Fo$(OBJ_DIR)/$(LOG4Z)/ -Fd$(OUTPUT_DIR)/$(LOG4Z)$(EXT) -c $<

$(LOG4Z): precheck $(LOG4Z_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(LOG4Z)$(EXT).dll" -dll $(LOG4Z_OBJS) $(UTILITIES)$(EXT).lib $(LIBS)

#=======================================================================resprovider-7zip=======================================================================
#不清楚这里用批量处理不了,会把cursoricon.obj编译到zip的目录下去,还好不多;p
#{$(RES_7Z_DIR)}.cpp.obj:
#	$(CXX) $(CXXFLAGS) $(RES_7Z_FLAGS) -Fo$(OBJ_DIR)/$(RES_7Z)/ -Fd$(OUTPUT_DIR)/$(RES_7Z) -c $<

$(OBJ_DIR)/$(RES_7Z)/SResProvider7Zip.obj: $(RES_7Z_DIR)/SResProvider7Zip.cpp
	$(CXX) $(CXXFLAGS) $(RES_7Z_FLAGS) -Fo$(OBJ_DIR)/$(RES_7Z)/ -Fd$(OUTPUT_DIR)/$(RES_7Z)$(EXT) -c $(RES_7Z_DIR)/SResProvider7Zip.cpp

$(OBJ_DIR)/$(RES_7Z)/cursoricon.obj: $(RES_7Z_DIR)/cursoricon.cpp
	$(CXX) $(CXXFLAGS) $(RES_7Z_FLAGS) -Fo$(OBJ_DIR)/$(RES_7Z)/ -Fd$(OUTPUT_DIR)/$(RES_7Z)$(EXT) -c $(RES_7Z_DIR)/cursoricon.cpp

$(OBJ_DIR)/$(RES_7Z)/zip7Archive.obj: $(RES_7Z_DIR)/zip7Archive.cpp
	$(CXX) $(CXXFLAGS) $(RES_7Z_FLAGS) -Fo$(OBJ_DIR)/$(RES_7Z)/ -Fd$(OUTPUT_DIR)/$(RES_7Z)$(EXT) -c $(RES_7Z_DIR)/zip7Archive.cpp

$(RES_7Z): precheck $(7Z) $(RES_7Z_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(RES_7Z)$(EXT).dll" -dll $(RES_7Z_OBJS) $(7Z)$(EXT).lib $(UTILITIES)$(EXT).lib $(LIBS)

#=======================================================================resprovider-zip=======================================================================

{$(RES_ZIP_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(RES_ZIP_FLAGS) -Fo$(OBJ_DIR)/$(RES_ZIP)/ -Fd$(OUTPUT_DIR)/$(RES_ZIP)$(EXT) -c $<

$(RES_ZIP): precheck $(ZLIB) $(RES_ZIP_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(RES_ZIP)$(EXT).dll" -dll $(RES_ZIP_OBJS) $(ZLIB)$(EXT).lib $(UTILITIES)$(EXT).lib $(LIBS)

#=======================================================================translator=======================================================================

{$(TRANSLATOR_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(TRANSLATOR_FLAGS) -Fo$(OBJ_DIR)/$(TRANSLATOR)/ -Fd$(OUTPUT_DIR)/$(TRANSLATOR)$(EXT) -c $<

$(TRANSLATOR): precheck $(TRANSLATOR_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(TRANSLATOR)$(EXT).dll" -dll $(TRANSLATOR_OBJS) $(UTILITIES)$(EXT).lib $(LIBS)

#=======================================================================scriptmodule-lua=======================================================================

{$(SCRIPTMODULE_DIR)/lua_tinker}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SCRIPTMODULE_FLAGS) -Fo$(OBJ_DIR)/$(SCRIPTMODULE)/ -Fd$(OUTPUT_DIR)/$(SCRIPTMODULE)$(EXT) -c $<

{$(SCRIPTMODULE_SRC)/exports}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SCRIPTMODULE_FLAGS) -Fo$(OBJ_DIR)/$(SCRIPTMODULE)/ -Fd$(OUTPUT_DIR)/$(SCRIPTMODULE)$(EXT) -c $<

{$(SCRIPTMODULE_SRC)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SCRIPTMODULE_FLAGS) -Fo$(OBJ_DIR)/$(SCRIPTMODULE)/ -Fd$(OUTPUT_DIR)/$(SCRIPTMODULE)$(EXT) -c $<

$(SCRIPTMODULE): precheck $(LUA) $(SCRIPTMODULE_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(SCRIPTMODULE)$(EXT).dll" -dll $(SCRIPTMODULE_OBJS) $(LUA)$(EXT).lib $(SOUI)$(EXT).lib $(UTILITIES)$(EXT).lib $(LIBS)

#=======================================================================render-gdi=======================================================================

{$(RENDER_GDI_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(RENDER_GDI_FLAGS) -Fo$(OBJ_DIR)/$(RENDER_GDI)/ -Fd$(OUTPUT_DIR)/$(RENDER_GDI)$(EXT) -c $<

$(RENDER_GDI): precheck $(RENDER_GDI_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(RENDER_GDI).dll" -dll $(RENDER_GDI_OBJS) $(UTILITIES)$(EXT).lib $(LIBS)

#=======================================================================render-skia=======================================================================

{$(RENDER_SKIA_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(RENDER_SKIA_FLAGS) -Fo$(OBJ_DIR)/$(RENDER_SKIA)/ -Fd$(OUTPUT_DIR)/$(RENDER_SKIA)$(EXT) -c $<

$(RENDER_SKIA): precheck $(SKIA) $(RENDER_SKIA_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(RENDER_SKIA)$(EXT).dll" -dll $(RENDER_SKIA_OBJS) $(SKIA)$(EXT).lib $(UTILITIES)$(EXT).lib shlwapi.lib Usp10.lib opengl32.lib $(LIBS)

#=======================================================================imgdecoder-png=======================================================================

{$(IMG_DECODER_PNG_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(IMG_DECODER_PNG_FLAGS) -Fo$(OBJ_DIR)/$(IMG_DECODER_PNG)/ -Fd$(OUTPUT_DIR)/$(IMG_DECODER_PNG)$(EXT) -c $<

$(IMG_DECODER_PNG): precheck $(ZLIB) $(PNG) $(IMG_DECODER_PNG_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(IMG_DECODER_PNG)$(EXT).dll" -dll $(IMG_DECODER_PNG_OBJS) $(PNG)$(EXT).lib $(ZLIB)$(EXT).lib $(LIBS)

#=======================================================================imgdecoder-gdip=======================================================================

$(OBJ_DIR)/$(IMG_DECODER_GDIP)/imgdecoder-gdip.obj: $(IMG_DECODER_GDIP_DIR)/imgdecoder-gdip.cpp
	$(CXX) $(CXXFLAGS) $(IMG_DECODER_GDIP_FLAGS) -Fo$(OBJ_DIR)/$(IMG_DECODER_GDIP)/ -Fd$(OUTPUT_DIR)/$(IMG_DECODER_GDIP)$(EXT) -c $(IMG_DECODER_GDIP_DIR)/imgdecoder-gdip.cpp

$(IMG_DECODER_GDIP): precheck $(IMG_DECODER_GDIP_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(IMG_DECODER_GDIP)$(EXT).dll" -dll $(IMG_DECODER_GDIP_OBJS) $(LIBS)

#=======================================================================imgdecoder-gdip=======================================================================

{$(IMG_DECODER_STB_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(IMG_DECODER_STB_FLAGS) -Fo$(OBJ_DIR)/$(IMG_DECODER_STB)/ -Fd$(OUTPUT_DIR)/$(IMG_DECODER_STB)$(EXT) -c $<

$(IMG_DECODER_STB): precheck $(IMG_DECODER_STB_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(IMG_DECODER_STB)$(EXT).dll" -dll $(IMG_DECODER_STB_OBJS) $(LIBS)

#=======================================================================imgdecoder-gdip=======================================================================

{$(IMG_DECODER_WIC_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(IMG_DECODER_WIC_FLAGS) -Fo$(OBJ_DIR)/$(IMG_DECODER_WIC)/ -Fd$(OUTPUT_DIR)/$(IMG_DECODER_WIC)$(EXT) -c $<

$(IMG_DECODER_WIC): precheck $(IMG_DECODER_WIC_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(IMG_DECODER_WIC)$(EXT).dll" -dll $(IMG_DECODER_WIC_OBJS) $(LIBS)

#=======================================================================imgdecoder-gdip=======================================================================

{$(DEMO_DIR)}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/appledock}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/clock}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<	

{$(DEMO_DIR)/magnet}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/skin}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/httpsvr}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/trayicon}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/qrcode}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/uianimation}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/../controls.extend}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/../controls.extend/propgrid}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/../controls.extend/propgrid/propitem}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/../controls.extend/reole}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/../controls.extend/SMcListViewEx}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

{$(DEMO_DIR)/../controls.extend/gif}.cpp.obj:
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) -Fo$(OBJ_DIR)/$(DEMO)/ -Fd$(OUTPUT_DIR)/$(DEMO)$(EXT) -c $<

$(OBJ_DIR)/$(DEMO)/$(DEMO).res: $(DEMO_DIR)/$(DEMO).rc
	$(RC) -I$(DEMO_DIR) -Fo"$(OBJ_DIR)/$(DEMO)/$(DEMO).res" $(DEMO_DIR)/$(DEMO).rc

$(DEMO): precheck predemo $(SCRIPTMODULE) $(DEMO_OBJS)
	$(LD) $(LDFLAGS) -out:"$(OUTPUT_DIR)/$(DEMO)$(EXT).exe" $(DEMO_OBJS) $(UTILITIES)$(EXT).lib $(SOUI)$(EXT).lib $(MHOOK)$(EXT).lib $(SMILEY)$(EXT).lib $(LIBS)

clean:
	if exist $(OBJ_DIR) rd /S/Q $(OBJ_DIR)\.
	if exist $(OUTPUT_DIR) rd /S/Q $(OUTPUT_DIR)\.

precheck:
	copy "config\options\dll.h" "config\core-def.h"
	copy "config\options\com-dll.h" "config\com-def.h"
	if not exist $(OUTPUT_DIR) mkdir $(OUTPUT_DIR)
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	if not exist $(OBJ_DIR)\$(UTILITIES) mkdir $(OBJ_DIR)\$(UTILITIES)
	if not exist $(OBJ_DIR)\$(SOUI) mkdir $(OBJ_DIR)\$(SOUI)
	if not exist $(OBJ_DIR)\$(SOUI_RESOURCE) mkdir $(OBJ_DIR)\$(SOUI_RESOURCE)
	if not exist $(OBJ_DIR)\$(ZLIB) mkdir $(OBJ_DIR)\$(ZLIB)
	if not exist $(OBJ_DIR)\$(PNG) mkdir $(OBJ_DIR)\$(PNG)
	if not exist $(OBJ_DIR)\$(SMILEY) mkdir $(OBJ_DIR)\$(SMILEY)
	if not exist $(OBJ_DIR)\$(MHOOK) mkdir $(OBJ_DIR)\$(MHOOK)
	if not exist $(OBJ_DIR)\$(GTEST) mkdir $(OBJ_DIR)\$(GTEST)
	if not exist $(OBJ_DIR)\$(LUA) mkdir $(OBJ_DIR)\$(LUA)
	if not exist $(OBJ_DIR)\$(JSONCPP) mkdir $(OBJ_DIR)\$(JSONCPP)
	if not exist $(OBJ_DIR)\$(SCINTILLA) mkdir $(OBJ_DIR)\$(SCINTILLA)
	if not exist $(OBJ_DIR)\$(7Z) mkdir $(OBJ_DIR)\$(7Z)
	if not exist $(OBJ_DIR)\$(SKIA) mkdir $(OBJ_DIR)\$(SKIA)
	if not exist $(OBJ_DIR)\$(LOG4Z) mkdir $(OBJ_DIR)\$(LOG4Z)
	if not exist $(OBJ_DIR)\$(RES_7Z) mkdir $(OBJ_DIR)\$(RES_7Z)
	if not exist $(OBJ_DIR)\$(RES_ZIP) mkdir $(OBJ_DIR)\$(RES_ZIP)
	if not exist $(OBJ_DIR)\$(TRANSLATOR) mkdir $(OBJ_DIR)\$(TRANSLATOR)
	if not exist $(OBJ_DIR)\$(SCRIPTMODULE) mkdir $(OBJ_DIR)\$(SCRIPTMODULE)
	if not exist $(OBJ_DIR)\$(RENDER_GDI) mkdir $(OBJ_DIR)\$(RENDER_GDI)
	if not exist $(OBJ_DIR)\$(RENDER_SKIA) mkdir $(OBJ_DIR)\$(RENDER_SKIA)
	if not exist $(OBJ_DIR)\$(IMG_DECODER_PNG) mkdir $(OBJ_DIR)\$(IMG_DECODER_PNG)
	if not exist $(OBJ_DIR)\$(IMG_DECODER_GDIP) mkdir $(OBJ_DIR)\$(IMG_DECODER_GDIP)
	if not exist $(OBJ_DIR)\$(IMG_DECODER_STB) mkdir $(OBJ_DIR)\$(IMG_DECODER_STB)
	if not exist $(OBJ_DIR)\$(IMG_DECODER_WIC) mkdir $(OBJ_DIR)\$(IMG_DECODER_WIC)
	if not exist $(OBJ_DIR)\$(DEMO) mkdir $(OBJ_DIR)\$(DEMO)

presoui:
	if not exist $(SOUI)\include\souistd.h.cpp echo #include "souistd.h">>$(SOUI)\include\souistd.h.cpp

predemo:
	cd $(DEMO_DIR)
	..\tools\uiresbuilder.exe -i "uires/uires.idx" -p uires -r .\res\soui_res.rc2 -h .\res\resource.h idtable
	cd ..

rebuild: clean all