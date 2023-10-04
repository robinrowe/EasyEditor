#include <QSvgRenderer>

#include "common_headers.h"
#include "MyRichEdit.h"
#include "PanelFindReplace.h"
#include "DlgScaleImg.h"
#include "DlgParaMargin.h"

#include "MenuColor.h"
#include "MenuUnderline.h"

#include "MacUnifiedToolBar.h"

#include "MainWindow.h"

static QFileDialog::Options fdo;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_pCurrentEdit(NULL)

	, m_bTextFamilySetable(true)
	, m_bTextSizeSetable(true)
{
	QMainWindow::setWindowIcon(QIcon(":/images/ico_app_logo.ico"));

	m_pTabDocuments = new QTabWidget(this);
	m_pTabDocuments->setDocumentMode(true);
	m_pTabDocuments->setTabsClosable(true);
	m_pTabDocuments->setMovable(true);
	QObject::connect(m_pTabDocuments, SIGNAL(currentChanged(int)), this, SLOT(onSwitchTab(int)));
	QObject::connect(m_pTabDocuments, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));

	m_pPanelFindReplace = new QPanelFindReplace(this);
	m_pPanelFindReplace->setVisible(false);
	QObject::connect(m_pPanelFindReplace, SIGNAL(visibilityChanged(bool)), this, SLOT(onFindPanelVisibilityChanged(bool)));

	QVBoxLayout* pLayoutMain = new QVBoxLayout;
	pLayoutMain->setSpacing(0);
	pLayoutMain->setContentsMargins(0, 0, 0, 0);
	pLayoutMain->addWidget(m_pTabDocuments);
	pLayoutMain->addWidget(m_pPanelFindReplace);

	QWidget* pWidget = new QWidget(this);
	pWidget->setLayout(pLayoutMain);

	QMainWindow::setCentralWidget(pWidget);
	QMainWindow::setUnifiedTitleAndToolBarOnMac(true);
//	QMainWindow::showMaximized();

	QStatusBar* pStatusBar = QMainWindow::statusBar();
	pStatusBar->setSizeGripEnabled(true);
	pStatusBar->setVisible(true);

	m_pDisplayDefault = new QLabel("EasyEditor 1.0.0", this);
	pStatusBar->addWidget(m_pDisplayDefault);
	m_pDisplayExtraInfo = new QLabel(this);
	pStatusBar->addPermanentWidget(m_pDisplayExtraInfo);

	m_pActionNew = new QAction(this);
	m_pActionNew->setText("New File ...");
	m_pActionNew->setIcon(g_xOpt.getIconForToolBar("btn_newfile"));
	m_pActionNew->setShortcut(QKeySequence(QKeySequence::New));
	QObject::connect(m_pActionNew, SIGNAL(triggered(bool)), this, SLOT(onNewFile()));

	m_pActionOpen = new QAction(this);
	m_pActionOpen->setText("Open File ...");
	m_pActionOpen->setIcon(g_xOpt.getIconForToolBar("btn_open"));
	m_pActionOpen->setShortcut(QKeySequence(QKeySequence::Open));
	QObject::connect(m_pActionOpen, SIGNAL(triggered(bool)), this, SLOT(onOpenFile()));

	m_pActionSave = new QAction(this);
	m_pActionSave->setText("Save File");
	m_pActionSave->setEnabled(false);
	m_pActionSave->setIcon(g_xOpt.getIconForToolBar("btn_save"));
	m_pActionSave->setShortcut(QKeySequence(QKeySequence::Save));
	QObject::connect(m_pActionSave, SIGNAL(triggered(bool)), this, SLOT(onSaveFile()));

	m_pActionExportAsHtml = new QAction(this);
	m_pActionExportAsHtml->setText("HTML Document ...");
	m_pActionExportAsHtml->setEnabled(false);
	QObject::connect(m_pActionExportAsHtml, SIGNAL(triggered(bool)), this, SLOT(onExportAsHtml()));

	m_pActionExportAsPdf = new QAction(this);
	m_pActionExportAsPdf->setText("PDF Document ...");
	m_pActionExportAsPdf->setEnabled(false);
	QObject::connect(m_pActionExportAsPdf, SIGNAL(triggered(bool)), this, SLOT(onExportAsPdf()));

	m_pActionExportAsText = new QAction(this);
	m_pActionExportAsText->setText("Plain Text ...");
	m_pActionExportAsText->setEnabled(false);
	QObject::connect(m_pActionExportAsText, SIGNAL(triggered(bool)), this, SLOT(onExportAsText()));

	m_pActionUndo = new QAction(this);
	m_pActionUndo->setEnabled(false);
	m_pActionUndo->setText("Undo");
	m_pActionUndo->setIcon(g_xOpt.getIconForToolBar("btn_undo"));
	m_pActionUndo->setShortcut(QKeySequence(QKeySequence::Undo));

	m_pActionRedo = new QAction(this);
	m_pActionRedo->setEnabled(false);
	m_pActionRedo->setText("Redo");
	m_pActionRedo->setIcon(g_xOpt.getIconForToolBar("btn_redo"));
	m_pActionRedo->setShortcut(QKeySequence(QKeySequence::Redo));

	m_pActionCut = new QAction(this);
	m_pActionCut->setText("Cut");
	m_pActionCut->setIcon(g_xOpt.getIconForToolBar("btn_cut"));
	m_pActionCut->setShortcut(QKeySequence(QKeySequence::Cut));

	m_pActionCopy = new QAction(this);
	m_pActionCopy->setText("Copy");
	m_pActionCopy->setIcon(g_xOpt.getIconForToolBar("btn_copy"));
	m_pActionCopy->setShortcut(QKeySequence(QKeySequence::Copy));

	m_pActionPaste = new QAction(this);
	m_pActionPaste->setText("Paste");
	m_pActionPaste->setIcon(g_xOpt.getIconForToolBar("btn_paste"));
	m_pActionPaste->setShortcut(QKeySequence(QKeySequence::Paste));

	m_pActionFindText = new QAction(this);
	m_pActionFindText->setText("Find ...");
	m_pActionFindText->setIcon(g_xOpt.getIconForToolBar("btn_find"));
	m_pActionFindText->setShortcut(QKeySequence(QKeySequence::Find));
	QObject::connect(m_pActionFindText, SIGNAL(triggered(bool)), this, SLOT(onFindText()));

	m_pActionReplaceText = new QAction(this);
	m_pActionReplaceText->setText("Replace ...");
	m_pActionReplaceText->setShortcut(QKeySequence(QKeySequence::Replace));
	QObject::connect(m_pActionReplaceText, SIGNAL(triggered(bool)), this, SLOT(onReplaceText()));

	m_pActionEditHyperlink = new QAction(this);
	m_pActionEditHyperlink->setText("Edit Hyperlink ...");
	m_pActionEditHyperlink->setIcon(g_xOpt.getIconForToolBar("btn_hyperlink_edit"));
	QObject::connect(m_pActionEditHyperlink, SIGNAL(triggered(bool)), this, SLOT(onEditHyperlink()));

	m_pActionCancelHyperlink = new QAction(this);
	m_pActionCancelHyperlink->setEnabled(false);
	m_pActionCancelHyperlink->setText("Cancel Hyperlink");
	QObject::connect(m_pActionCancelHyperlink, SIGNAL(triggered(bool)), this, SLOT(onCancelHyperlink()));

	m_pActionFont = new QAction(this);
	m_pActionFont->setText("Font ...");
	m_pActionFont->setIcon(g_xOpt.getIconForToolBar("btn_font"));
	QObject::connect(m_pActionFont, SIGNAL(triggered(bool)), this, SLOT(onCustomFont()));

	m_pActionBold = new QAction(this);
	m_pActionBold->setCheckable(true);
	m_pActionBold->setText("Bold");
	m_pActionBold->setIcon(g_xOpt.getIconForToolBar("btn_bold"));
	m_pActionBold->setShortcut(QKeySequence(QKeySequence::Bold));

	m_pActionItalic = new QAction(this);
	m_pActionItalic->setCheckable(true);
	m_pActionItalic->setText("Italic");
	m_pActionItalic->setIcon(g_xOpt.getIconForToolBar("btn_italic"));
	m_pActionItalic->setShortcut(QKeySequence(QKeySequence::Italic));

	m_pActionUnderline = new QAction(this);
	m_pActionUnderline->setCheckable(true);
	m_pActionUnderline->setText("Underline");
	m_pActionUnderline->setIcon(g_xOpt.getIconForToolBar("btn_underline"));
	m_pActionUnderline->setShortcut(QKeySequence(QKeySequence::Underline));
	QObject::connect(m_pActionUnderline, SIGNAL(triggered(bool)), this, SLOT(onUnderline()));
	QToolButton* pBtnUnderline = new QToolButton(this);
	pBtnUnderline->setDefaultAction(m_pActionUnderline);
	pBtnUnderline->setCheckable(true);
	{
		m_pMenuUnderline = new QMenuUnderline(this);
		pBtnUnderline->setPopupMode(QToolButton::MenuButtonPopup);
		pBtnUnderline->setMenu(m_pMenuUnderline);
		QObject::connect(m_pMenuUnderline, SIGNAL(colorChose(QColor)), this, SLOT(onSetUnderlineColor(QColor)));
		QObject::connect(m_pMenuUnderline, SIGNAL(styleChose(QTextCharFormat::UnderlineStyle)), this, SLOT(onSetUnderlineStyle(QTextCharFormat::UnderlineStyle)));
	}

	m_pActionOverline = new QAction(this);
	m_pActionOverline->setCheckable(true);
	m_pActionOverline->setText("Overline");
	m_pActionOverline->setIcon(g_xOpt.getIconForToolBar("btn_overline"));

	m_pActionStrikeOut = new QAction(this);
	m_pActionStrikeOut->setCheckable(true);
	m_pActionStrikeOut->setText("Strike Out");
	m_pActionStrikeOut->setIcon(g_xOpt.getIconForToolBar("btn_strikeout"));

	m_pActionSubscript = new QAction(this);
	m_pActionSubscript->setCheckable(true);
	m_pActionSubscript->setText("Subscript");
	m_pActionSubscript->setIcon(g_xOpt.getIconForToolBar("btn_subscript"));

	m_pActionSuperscript = new QAction(this);
	m_pActionSuperscript->setCheckable(true);
	m_pActionSuperscript->setText("Superscript");
	m_pActionSuperscript->setIcon(g_xOpt.getIconForToolBar("btn_superscript"));

	m_pComboTextFamily = new QFontComboBox(this);
	m_pComboTextFamily->setMaxVisibleItems(20);
	m_pComboTextFamily->setCurrentIndex(-1);
	QObject::connect(m_pComboTextFamily, SIGNAL(currentFontChanged(QFont)), this, SLOT(onTextFamily(QFont)));

	m_pComboTextSize = new QComboBox(this);
	m_pComboTextSize->setEditable(true);
	{
		QList<int> vSizes;
		vSizes << 9 << 10 << 11 << 12 << 13 << 14 << 15 << 16 << 17 << 18 << 19 << 20 << 24 << 28 << 32 << 36 << 48 << 64;
		Q_FOREACH(int nSize, vSizes){
			m_pComboTextSize->addItem(QString::number(nSize), QVariant(nSize));
		}
		m_pComboTextSize->setMaxVisibleItems(vSizes.size());
		m_pComboTextSize->setValidator(new QIntValidator(9, 99, this));
		m_pComboTextSize->setCurrentIndex(-1);
	}
//	QObject::connect(m_pComboTextSize, SIGNAL(currentIndexChanged(int)), this, SLOT(onTextSize(int)));
	QObject::connect(m_pComboTextSize, SIGNAL(editTextChanged(QString)), this, SLOT(onTextSize(QString)));

	m_pActionTextForeColor = new QActionColor("Foreground Color ...", g_xOpt.getIconForToolBar("btn_forecolor"), QColor(Qt::black), this);
	m_pActionTextForeColor->setTriggerAction(QActionColor::PickColorFromDlg);
	m_pActionTextForeColor->setEnabled(false);

	m_pActionTextBackColor = new QActionColor("Background Color ...", g_xOpt.getIconForToolBar("btn_backcolor"), QColor(Qt::transparent), this);
	m_pActionTextBackColor->setTriggerAction(QActionColor::PickColorFromDlg);
	m_pActionTextBackColor->setEnabled(false);

	m_pBtnTextForeColor = new QToolButtonColor(g_xOpt.getIconForToolBar("btn_forecolor"), QColor(Qt::black), this);
	m_pBtnTextForeColor->setToolButtonStyle(Qt::ToolButtonIconOnly);
	QObject::connect(m_pActionTextForeColor, SIGNAL(colorChose(QColor)), m_pBtnTextForeColor, SLOT(setCurrentColor(QColor)));
	QObject::connect(m_pBtnTextForeColor, SIGNAL(colorChose(QColor)), m_pActionTextForeColor, SLOT(setCurrentColor(QColor)));

	m_pBtnTextBackColor = new QToolButtonColor(g_xOpt.getIconForToolBar("btn_backcolor"), QColor(Qt::transparent), this);
	m_pBtnTextBackColor->setToolButtonStyle(Qt::ToolButtonIconOnly);
	QObject::connect(m_pActionTextBackColor, SIGNAL(colorChose(QColor)), m_pBtnTextBackColor, SLOT(setCurrentColor(QColor)));
	QObject::connect(m_pBtnTextBackColor, SIGNAL(colorChose(QColor)), m_pActionTextBackColor, SLOT(setCurrentColor(QColor)));

	m_pActionJustifyLeft = new QAction(this);
	m_pActionJustifyLeft->setCheckable(true);
	m_pActionJustifyLeft->setText("Justify Left");
	m_pActionJustifyLeft->setIcon(g_xOpt.getIconForToolBar("btn_justifyleft"));

	m_pActionJustifyCenter = new QAction(this);
	m_pActionJustifyCenter->setCheckable(true);
	m_pActionJustifyCenter->setText("Justify Center");
	m_pActionJustifyCenter->setIcon(g_xOpt.getIconForToolBar("btn_justifycenter"));

	m_pActionJustifyRight = new QAction(this);
	m_pActionJustifyRight->setCheckable(true);
	m_pActionJustifyRight->setText("Justify Right");
	m_pActionJustifyRight->setIcon(g_xOpt.getIconForToolBar("btn_justifyright"));

	m_pActionParaMargin = new QAction(this);
	m_pActionParaMargin->setText("Paragraph Margin");
	QObject::connect(m_pActionParaMargin, SIGNAL(triggered(bool)), this, SLOT(onParaMargin()));

	m_pActionIndent = new QAction(this);
	m_pActionIndent->setText("Indent");
	m_pActionIndent->setIcon(g_xOpt.getIconForToolBar("btn_indent"));

	m_pActionOutdent = new QAction(this);
	m_pActionOutdent->setText("Outdent");
	m_pActionOutdent->setIcon(g_xOpt.getIconForToolBar("btn_outdent"));

	m_pActionLineSpacingx1 = new QAction(this);
	m_pActionLineSpacingx1->setCheckable(true);
	m_pActionLineSpacingx1->setText("Single Line Spacing");
	m_pActionLineSpacingx1->setIcon(g_xOpt.getIconForToolBar("btn_linespacingx1"));

	m_pActionLineSpacingx15 = new QAction(this);
	m_pActionLineSpacingx15->setCheckable(true);
	m_pActionLineSpacingx15->setText("One and Half Line Spacing");
	m_pActionLineSpacingx15->setIcon(g_xOpt.getIconForToolBar("btn_linespacingx15"));

	m_pActionLineSpacingx2 = new QAction(this);
	m_pActionLineSpacingx2->setCheckable(true);
	m_pActionLineSpacingx2->setText("Double Line Spacing");
	m_pActionLineSpacingx2->setIcon(g_xOpt.getIconForToolBar("btn_linespacingx2"));

	m_pActionGrpBlockList = new QActionGroup(this);
	m_pActionGrpBlockList->setExclusive(true);

	m_pActionInsOrderedList = new QAction(this);
	m_pActionInsOrderedList->setCheckable(true);
	m_pActionInsOrderedList->setText("Ordered List");
	m_pActionInsOrderedList->setIcon(g_xOpt.getIconForToolBar("btn_ordered_list"));
	QToolButton* pBtnInsOrderedList = new QToolButton(this);
	pBtnInsOrderedList->setDefaultAction(m_pActionInsOrderedList);
	pBtnInsOrderedList->setCheckable(true);
	{
		_CPairVector<QString, QTextListFormat::Style> vStyles;
		vStyles["Decimal"] = QTextListFormat::ListDecimal;
		vStyles["Lower Alpha"] = QTextListFormat::ListLowerAlpha;
		vStyles["Upper Alpha"] = QTextListFormat::ListUpperAlpha;
		vStyles["Lower Roman"] = QTextListFormat::ListLowerRoman;
		vStyles["Upper Roman"] = QTextListFormat::ListUpperRoman;

		QMenu* pMenu = new QMenu(this);
		_CPairVector<QString, QTextListFormat::Style>::const_iterator it;
		__EACH(it, vStyles){
			QAction* pAction = new QAction(this);
			pAction->setText(it->first);
			pAction->setData(QVariant(it->second));
			pAction->setCheckable(true);
			QObject::connect(pAction, SIGNAL(triggered(bool)), this, SLOT(onSetListStyle()));
			pMenu->addAction(pAction);
			m_pActionGrpBlockList->addAction(pAction);
		}

		pBtnInsOrderedList->setPopupMode(QToolButton::MenuButtonPopup);
		pBtnInsOrderedList->setMenu(pMenu);
	}

	m_pActionInsUnorderedList = new QAction(this);
	m_pActionInsUnorderedList->setCheckable(true);
	m_pActionInsUnorderedList->setText("Unordered List");
	m_pActionInsUnorderedList->setIcon(g_xOpt.getIconForToolBar("btn_unordered_list"));
	QToolButton* pBtnInsUnorderedList = new QToolButton(this);
	pBtnInsUnorderedList->setDefaultAction(m_pActionInsUnorderedList);
	pBtnInsUnorderedList->setCheckable(true);
	{
		_CPairVector<QString, QTextListFormat::Style> vStyles;
		vStyles["Disc"] = QTextListFormat::ListDisc;
		vStyles["Circle"] = QTextListFormat::ListCircle;
		vStyles["Square"] = QTextListFormat::ListSquare;

		QMenu* pMenu = new QMenu(this);
		_CPairVector<QString, QTextListFormat::Style>::const_iterator it;
		__EACH(it, vStyles){
			QAction* pAction = new QAction(this);
			pAction->setText(it->first);
			pAction->setData(QVariant(it->second));
			pAction->setCheckable(true);
			QObject::connect(pAction, SIGNAL(triggered(bool)), this, SLOT(onSetListStyle()));
			pMenu->addAction(pAction);
			m_pActionGrpBlockList->addAction(pAction);
		}

		pBtnInsUnorderedList->setPopupMode(QToolButton::MenuButtonPopup);
		pBtnInsUnorderedList->setMenu(pMenu);
	}

	m_pActionInsImage = new QAction(this);
	m_pActionInsImage->setText("Insert Images ...");
	m_pActionInsImage->setIcon(g_xOpt.getIconForToolBar("btn_ins_img"));
	QObject::connect(m_pActionInsImage, SIGNAL(triggered(bool)), this, SLOT(onInsertImage()));

	m_pActionExportImage = new QAction(this);
	m_pActionExportImage->setEnabled(false);
	m_pActionExportImage->setText("Export Image ...");
	QObject::connect(m_pActionExportImage, SIGNAL(triggered(bool)), this, SLOT(onExportImage()));

	m_pActionRotateImage = new QAction(this);
	m_pActionRotateImage->setEnabled(false);
	m_pActionRotateImage->setText("Rotate Image");
	m_pActionRotateImage->setIcon(g_xOpt.getIconForToolBar("btn_rotate_img"));
	QObject::connect(m_pActionRotateImage, SIGNAL(triggered(bool)), this, SLOT(onRotateImage()));

	m_pActionScaleImage = new QAction(this);
	m_pActionScaleImage->setEnabled(false);
	m_pActionScaleImage->setText("Scale Image");
	m_pActionScaleImage->setIcon(g_xOpt.getIconForToolBar("btn_scale_img"));
	QObject::connect(m_pActionScaleImage, SIGNAL(triggered(bool)), this, SLOT(onScaleImage()));

	m_pActionInsTable = new QAction(this);
	m_pActionInsTable->setText("Insert Table");
	m_pActionInsTable->setIcon(g_xOpt.getIconForToolBar("btn_ins_table"));
	QObject::connect(m_pActionInsTable, SIGNAL(triggered(bool)), this, SLOT(onInsertTable()));

	m_pActionInsRowBefore = new QAction(this);
	m_pActionInsRowBefore->setEnabled(false);
	m_pActionInsRowBefore->setText("Insert Row Before");
	m_pActionInsRowBefore->setIcon(g_xOpt.getIconForToolBar("btn_ins_row_before"));

	m_pActionInsRowAfter = new QAction(this);
	m_pActionInsRowAfter->setEnabled(false);
	m_pActionInsRowAfter->setText("Insert Row After");
	m_pActionInsRowAfter->setIcon(g_xOpt.getIconForToolBar("btn_ins_row_after"));

	m_pActionInsColBefore = new QAction(this);
	m_pActionInsColBefore->setEnabled(false);
	m_pActionInsColBefore->setText("Insert Column Before");
	m_pActionInsColBefore->setIcon(g_xOpt.getIconForToolBar("btn_ins_col_before"));

	m_pActionInsColAfter = new QAction(this);
	m_pActionInsColAfter->setEnabled(false);
	m_pActionInsColAfter->setText("Insert Column After");
	m_pActionInsColAfter->setIcon(g_xOpt.getIconForToolBar("btn_ins_col_after"));

	QToolButton* pBtnInsTableEle = new QToolButton(this);
	pBtnInsTableEle->setDefaultAction(m_pActionInsRowBefore);
	pBtnInsTableEle->setCheckable(true);
	{
		QMenu* pMenu = new QMenu(this);
		pMenu->addAction(m_pActionInsRowBefore);
		pMenu->addAction(m_pActionInsRowAfter);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionInsColBefore);
		pMenu->addAction(m_pActionInsColAfter);

		pBtnInsTableEle->setPopupMode(QToolButton::MenuButtonPopup);
		pBtnInsTableEle->setMenu(pMenu);
	}

	m_pActionRemoveRows = new QAction(this);
	m_pActionRemoveRows->setEnabled(false);
	m_pActionRemoveRows->setText("Remove Rows");
	m_pActionRemoveRows->setIcon(g_xOpt.getIconForToolBar("btn_remove_rows"));

	m_pActionRemoveCols = new QAction(this);
	m_pActionRemoveCols->setEnabled(false);
	m_pActionRemoveCols->setText("Remove Columns");
	m_pActionRemoveCols->setIcon(g_xOpt.getIconForToolBar("btn_remove_cols"));

	QToolButton* pBtnRemoveTableEle = new QToolButton(this);
	pBtnRemoveTableEle->setDefaultAction(m_pActionRemoveRows);
	pBtnRemoveTableEle->setCheckable(true);
	{
		QMenu* pMenu = new QMenu(this);
		pMenu->addAction(m_pActionRemoveRows);
		pMenu->addAction(m_pActionRemoveCols);

		pBtnRemoveTableEle->setPopupMode(QToolButton::MenuButtonPopup);
		pBtnRemoveTableEle->setMenu(pMenu);
	}

	m_pActionMergeCells = new QAction(this);
	m_pActionMergeCells->setEnabled(false);
	m_pActionMergeCells->setText("Merge Cells");
	m_pActionMergeCells->setIcon(g_xOpt.getIconForToolBar("btn_merge_cells"));

	m_pActionSplitCells = new QAction(this);
	m_pActionSplitCells->setEnabled(false);
	m_pActionSplitCells->setText("Split Cells");
	m_pActionSplitCells->setIcon(g_xOpt.getIconForToolBar("btn_split_cells"));

	m_pActionTableBorderColor = new QActionColor("Border ...", g_xOpt.getIconForToolBar("btn_table_border"), QColor(Qt::darkGray), this);
	m_pActionTableBorderColor->setTriggerAction(QActionColor::PickColorFromDlg);
	m_pActionTableBorderColor->setEnabled(false);

	m_pActionCellBackColor = new QActionColor("Fill ...", g_xOpt.getIconForToolBar("btn_cell_background"), QColor(Qt::transparent), this);
	m_pActionCellBackColor->setTriggerAction(QActionColor::PickColorFromDlg);
	m_pActionCellBackColor->setEnabled(false);

#if !defined(Q_OS_MAC)

	m_pBtnTableBorderColor = new QToolButtonColor(g_xOpt.getIconForToolBar("btn_table_border"), QColor(Qt::darkGray), this);
	m_pBtnTableBorderColor->setEnabled(false);
	m_pBtnTableBorderColor->setToolButtonStyle(Qt::ToolButtonIconOnly);
	QObject::connect(m_pActionTableBorderColor, SIGNAL(colorChose(QColor)), m_pBtnTableBorderColor, SLOT(setCurrentColor(QColor)));
	QObject::connect(m_pBtnTableBorderColor, SIGNAL(colorChose(QColor)), m_pActionTableBorderColor, SLOT(setCurrentColor(QColor)));

	m_pBtnCellBackColor = new QToolButtonColor(g_xOpt.getIconForToolBar("btn_cell_background"), QColor(Qt::transparent), this);
	m_pBtnCellBackColor->setEnabled(false);
	m_pBtnCellBackColor->setToolButtonStyle(Qt::ToolButtonIconOnly);
	QObject::connect(m_pActionCellBackColor, SIGNAL(colorChose(QColor)), m_pBtnCellBackColor, SLOT(setCurrentColor(QColor)));
	QObject::connect(m_pBtnCellBackColor, SIGNAL(colorChose(QColor)), m_pActionCellBackColor, SLOT(setCurrentColor(QColor)));

#endif

	m_pActionWidenCols = new QAction(this);
	m_pActionWidenCols->setEnabled(false);
	m_pActionWidenCols->setText("Widen Columns");
	m_pActionWidenCols->setIcon(g_xOpt.getIconForToolBar("btn_widen_cols"));
	QObject::connect(m_pActionWidenCols, SIGNAL(triggered(bool)), this, SLOT(onWidenSelCols()));

	m_pActionNarrowCols = new QAction(this);
	m_pActionNarrowCols->setEnabled(false);
	m_pActionNarrowCols->setText("Narrow Columns");
	m_pActionNarrowCols->setIcon(g_xOpt.getIconForToolBar("btn_narrow_cols"));
	QObject::connect(m_pActionNarrowCols, SIGNAL(triggered(bool)), this, SLOT(onNarrowSelCols()));

	m_pActionGitHub = new QAction(this);
	m_pActionGitHub->setText("EasyEditor on GitHub");
	QObject::connect(m_pActionGitHub, SIGNAL(triggered(bool)), this, SLOT(onViewGitHub()));

	m_pActionAboutQt = new QAction(this);
	m_pActionAboutQt->setMenuRole(QAction::AboutQtRole);
	m_pActionAboutQt->setText("About Qt ...");
	QObject::connect(m_pActionAboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

	m_pActionAbout = new QAction(this);
	m_pActionAbout->setMenuRole(QAction::AboutRole);
	m_pActionAbout->setText("About EasyEditor ...");
	QObject::connect(m_pActionAbout, SIGNAL(triggered(bool)), this, SLOT(onAbout()));


	//Menubar
	QMenuBar* pMB = QMainWindow::menuBar();
	{
		//File menu
		QMenu* pMenu = pMB->addMenu("&File");
		pMenu->addAction(m_pActionNew);
		pMenu->addAction(m_pActionOpen);
		pMenu->addAction(m_pActionSave);
		pMenu->addSeparator();
		{
			QMenu* pMenuExport = new QMenu("Export as ...", this);
			pMenuExport->addAction(m_pActionExportAsHtml);
			pMenuExport->addAction(m_pActionExportAsPdf);
			pMenuExport->addAction(m_pActionExportAsText);
			pMenu->addMenu(pMenuExport);
		}
	}
	{
		//Edit menu
		QMenu* pMenu = pMB->addMenu("&Edit");
		pMenu->addAction(m_pActionUndo);
		pMenu->addAction(m_pActionRedo);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionCut);
		pMenu->addAction(m_pActionCopy);
		pMenu->addAction(m_pActionPaste);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionFindText);
		pMenu->addAction(m_pActionReplaceText);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionEditHyperlink);
		pMenu->addAction(m_pActionCancelHyperlink);
	}
	{
		//Format menu
		QMenu* pMenu = pMB->addMenu("&Format");
		pMenu->addAction(m_pActionFont);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionBold);
		pMenu->addAction(m_pActionItalic);
		pMenu->addAction(m_pActionUnderline);
		pMenu->addAction(m_pActionOverline);
		pMenu->addAction(m_pActionStrikeOut);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionSubscript);
		pMenu->addAction(m_pActionSuperscript);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionTextForeColor);
		pMenu->addAction(m_pActionTextBackColor);
	}
	{
		//Paragraph menu
		QMenu* pMenu = pMB->addMenu("&Paragraph");
		pMenu->addAction(m_pActionJustifyLeft);
		pMenu->addAction(m_pActionJustifyCenter);
		pMenu->addAction(m_pActionJustifyRight);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionOutdent);
		pMenu->addAction(m_pActionIndent);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionInsOrderedList);
		pMenu->addAction(m_pActionInsUnorderedList);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionLineSpacingx1);
		pMenu->addAction(m_pActionLineSpacingx15);
		pMenu->addAction(m_pActionLineSpacingx2);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionParaMargin);
	}
	{
		//Image menu
		QMenu* pMenu = pMB->addMenu("&Image");
		pMenu->addAction(m_pActionInsImage);
		pMenu->addAction(m_pActionExportImage);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionRotateImage);
		pMenu->addAction(m_pActionScaleImage);
	}
	{
		//Table menu
		QMenu* pMenu = pMB->addMenu("&Table");
		pMenu->addAction(m_pActionInsTable);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionInsRowBefore);
		pMenu->addAction(m_pActionInsRowAfter);
		pMenu->addAction(m_pActionInsColBefore);
		pMenu->addAction(m_pActionInsColAfter);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionRemoveRows);
		pMenu->addAction(m_pActionRemoveCols);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionMergeCells);
		pMenu->addAction(m_pActionSplitCells);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionTableBorderColor);
		pMenu->addAction(m_pActionCellBackColor);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionWidenCols);
		pMenu->addAction(m_pActionNarrowCols);
	}
	{
		//About menu
		QMenu* pMenu = pMB->addMenu("&Help");
		pMenu->addAction(m_pActionGitHub);
		pMenu->addSeparator();
		pMenu->addAction(m_pActionAboutQt);
		pMenu->addAction(m_pActionAbout);
	}

	//Toolbar
	QMainWindow::setIconSize(g_xOpt.m_szIconSize);

#if defined(Q_OS_MAC)
	QUnifiedToolBar* pTB = new QUnifiedToolBar(this);
	pTB->setMovable(false);
	pTB->addAction("File", m_pActionNew);
	pTB->addAction("File", m_pActionOpen);
	pTB->addAction("File", m_pActionSave);

	pTB->addAction("Format", m_pActionBold);
	pTB->addAction("Format", m_pActionItalic);
	pTB->addWidget("Format", pBtnUnderline);

	pTB->addWidget("Font", m_pComboTextFamily);
	pTB->addWidget("Font", m_pComboTextSize);

	pTB->addWidget("Color", m_pBtnTextForeColor);
	pTB->addWidget("Color", m_pBtnTextBackColor);

	pTB->addAction("Alignment", m_pActionJustifyLeft);
	pTB->addAction("Alignment", m_pActionJustifyCenter);
	pTB->addAction("Alignment", m_pActionJustifyRight);

	pTB->addAction("Indent", m_pActionOutdent);
	pTB->addAction("Indent", m_pActionIndent);

	pTB->addWidget("List", pBtnInsOrderedList);
	pTB->addWidget("List", pBtnInsUnorderedList);

	pTB->addAction("Image", m_pActionInsImage);
	pTB->addAction("Image", m_pActionRotateImage);
	pTB->addAction("Image", m_pActionScaleImage);

	pTB->addAction("Table", m_pActionInsTable);
	pTB->addWidget("Table", pBtnInsTableEle);
	pTB->addWidget("Table", pBtnRemoveTableEle);

	QMainWindow::addToolBar(Qt::TopToolBarArea, pTB);
#else
	{
		QToolBar* pTB = QMainWindow::addToolBar("File");
		pTB->addAction(m_pActionNew);
		pTB->addAction(m_pActionOpen);
		pTB->addAction(m_pActionSave);
	}
	{
		QToolBar* pTB = QMainWindow::addToolBar("Edit");
		pTB->addAction(m_pActionUndo);
		pTB->addAction(m_pActionRedo);
		pTB->addSeparator();
		pTB->addAction(m_pActionCut);
		pTB->addAction(m_pActionCopy);
		pTB->addAction(m_pActionPaste);
	}
	{
		QToolBar* pTB = QMainWindow::addToolBar("Format");
		pTB->addAction(m_pActionBold);
		pTB->addAction(m_pActionItalic);
		pTB->addWidget(pBtnUnderline);
		pTB->addSeparator();
		pTB->addWidget(m_pComboTextFamily);
		pTB->addWidget(m_pComboTextSize);
		pTB->addSeparator();
		pTB->addWidget(m_pBtnTextForeColor);
		pTB->addWidget(m_pBtnTextBackColor);
	}
	{
		QToolBar* pTB = QMainWindow::addToolBar("Paragraph");
		pTB->addAction(m_pActionJustifyLeft);
		pTB->addAction(m_pActionJustifyCenter);
		pTB->addAction(m_pActionJustifyRight);
		pTB->addSeparator();
		pTB->addAction(m_pActionOutdent);
		pTB->addAction(m_pActionIndent);
		pTB->addSeparator();
		pTB->addWidget(pBtnInsOrderedList);
		pTB->addWidget(pBtnInsUnorderedList);
	}
	{
		QToolBar* pTB = QMainWindow::addToolBar("Image");
		pTB->addAction(m_pActionInsImage);
		pTB->addAction(m_pActionRotateImage);
		pTB->addAction(m_pActionScaleImage);
	}
	{
		QToolBar* pTB = QMainWindow::addToolBar("Table");
		pTB->addAction(m_pActionInsTable);
		pTB->addSeparator();
		pTB->addWidget(pBtnInsTableEle);
		pTB->addWidget(pBtnRemoveTableEle);
		pTB->addSeparator();
		pTB->addAction(m_pActionMergeCells);
		pTB->addAction(m_pActionSplitCells);
		pTB->addSeparator();
		pTB->addWidget(m_pBtnTableBorderColor);
		pTB->addWidget(m_pBtnCellBackColor);
	}
#endif

	onLoadRecentFiles();
	updateEditActionsState();
}

MainWindow::~MainWindow()
{

}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_Escape){
		if(m_pPanelFindReplace->isVisible()){
			m_pPanelFindReplace->setVisible(false);
		}
	}
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	bool bAccepted = true; QStringList vMru;
	for(int i = 0; i < m_pTabDocuments->count(); i++){
		_CMyRichEdit* pRichEdit = qobject_cast<_CMyRichEdit*>(m_pTabDocuments->widget(i));
		if(pRichEdit){
			if(!confirmSaveFile(i)){
				bAccepted = false;
				break;
			}
			if(!pRichEdit->filePath().isEmpty()) vMru << pRichEdit->filePath();
		}
	}
	if(bAccepted){
		g_xOpt.m_vMruFiles = vMru;

		QString sFnIni = _CLocalFile::changeExtension(QCoreApplication::applicationFilePath(), "ini");
		g_xOpt.saveIni(sFnIni);
	}
	e->setAccepted(bAccepted);
}

void MainWindow::makeUniqueConnection(const QObject* sender, const char* signal, const QObject* prevReceiver, const QObject* newReceiver, const char* member)
{
	if(prevReceiver) QObject::disconnect(sender, signal, prevReceiver, member);
	QObject::connect(sender, signal, newReceiver, member);
}

void MainWindow::makeUniqueConnection(const QObject* prevSender, const QObject* newSender, const char* signal, const QObject* receiver, const char* member)
{
	if(prevSender) QObject::disconnect(prevSender, signal, receiver, member);
	QObject::connect(newSender, signal, receiver, member);
}

void MainWindow::makeConnections(_CMyRichEdit* pPrevEdit, _CMyRichEdit* pNewEdit)
{
	if(pPrevEdit){
		makeUniqueConnection(m_pActionUndo, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(_undo()));
		makeUniqueConnection(m_pActionRedo, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(_redo()));
		makeUniqueConnection(m_pActionCut, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(cut()));
		makeUniqueConnection(m_pActionCopy, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(copy()));
		makeUniqueConnection(m_pActionPaste, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(paste()));
		makeUniqueConnection(m_pActionBold, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(toggleFontBold()));
		makeUniqueConnection(m_pActionItalic, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(toggleFontItalic()));
		makeUniqueConnection(m_pActionOverline, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(toggleFontOverline()));
		makeUniqueConnection(m_pActionStrikeOut, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(toggleFontStrikeOut()));
		makeUniqueConnection(m_pActionSubscript, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(toggleFontSubscript()));
		makeUniqueConnection(m_pActionSuperscript, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(toggleFontSuperscript()));
		makeUniqueConnection(m_pActionTextForeColor, SIGNAL(colorChose(QColor)), pPrevEdit, pNewEdit, SLOT(setTextForeground(QColor)));
		makeUniqueConnection(m_pActionTextBackColor, SIGNAL(colorChose(QColor)), pPrevEdit, pNewEdit, SLOT(setTextBackground(QColor)));
		makeUniqueConnection(m_pBtnTextForeColor, SIGNAL(colorChose(QColor)), pPrevEdit, pNewEdit, SLOT(setTextForeground(QColor)));
		makeUniqueConnection(m_pBtnTextBackColor, SIGNAL(colorChose(QColor)), pPrevEdit, pNewEdit, SLOT(setTextBackground(QColor)));
		makeUniqueConnection(m_pActionJustifyLeft, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(justifyLeft()));
		makeUniqueConnection(m_pActionJustifyCenter, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(justifyCenter()));
		makeUniqueConnection(m_pActionJustifyRight, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(justifyRight()));
		makeUniqueConnection(m_pActionIndent, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(indent()));
		makeUniqueConnection(m_pActionOutdent, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(outdent()));
		makeUniqueConnection(m_pActionLineSpacingx1, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(setLineSpacingx1()));
		makeUniqueConnection(m_pActionLineSpacingx15, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(setLineSpacingx15()));
		makeUniqueConnection(m_pActionLineSpacingx2, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(setLineSpacingx2()));
		makeUniqueConnection(m_pActionInsOrderedList, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(toggleOrderedList()));
		makeUniqueConnection(m_pActionInsUnorderedList, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(toggleUnorderedList()));
		makeUniqueConnection(m_pActionInsRowBefore, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(insertRowBefore()));
		makeUniqueConnection(m_pActionInsRowAfter, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(insertRowAfter()));
		makeUniqueConnection(m_pActionInsColBefore, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(insertColBefore()));
		makeUniqueConnection(m_pActionInsColAfter, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(insertColAfter()));
		makeUniqueConnection(m_pActionRemoveRows, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(removeRows()));
		makeUniqueConnection(m_pActionRemoveCols, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(removeCols()));
		makeUniqueConnection(m_pActionMergeCells, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(mergeTableCells()));
		makeUniqueConnection(m_pActionSplitCells, SIGNAL(triggered(bool)), pPrevEdit, pNewEdit, SLOT(splitTableCells()));
		makeUniqueConnection(m_pActionTableBorderColor, SIGNAL(colorChose(QColor)), pPrevEdit, pNewEdit, SLOT(setTableBorderColor(QColor)));
		makeUniqueConnection(m_pActionCellBackColor, SIGNAL(colorChose(QColor)), pPrevEdit, pNewEdit, SLOT(setCellsBackgroundColor(QColor)));
#if !defined(Q_OS_MAC)
		makeUniqueConnection(m_pBtnTableBorderColor, SIGNAL(colorChose(QColor)), pPrevEdit, pNewEdit, SLOT(setTableBorderColor(QColor)));
		makeUniqueConnection(m_pBtnCellBackColor, SIGNAL(colorChose(QColor)), pPrevEdit, pNewEdit, SLOT(setCellsBackgroundColor(QColor)));
#endif

		makeUniqueConnection(m_pPanelFindReplace, SIGNAL(highlightOccurrences(QString,bool,bool,bool)), pPrevEdit, pNewEdit, SLOT(highlightOccurrences(QString,bool,bool,bool)));
		makeUniqueConnection(m_pPanelFindReplace, SIGNAL(findNext(QString,bool,bool,bool,bool,bool)), pPrevEdit, pNewEdit, SLOT(findNext(QString,bool,bool,bool,bool,bool)));
		makeUniqueConnection(m_pPanelFindReplace, SIGNAL(replace(QString,QString,bool,bool,bool,bool)), pPrevEdit, pNewEdit, SLOT(replaceText(QString,QString,bool,bool,bool,bool)));
		makeUniqueConnection(m_pPanelFindReplace, SIGNAL(replaceAll(QString,QString,bool,bool,bool)), pPrevEdit, pNewEdit, SLOT(replaceAll(QString,QString,bool,bool,bool)));
	}
}

void MainWindow::createEdit(const QString& sFilePath)
{
	bool bFirst = (m_pTabDocuments->count() == 0);
	QFileInfo xFI(sFilePath);
	QString sTitle = xFI.baseName(); if(sTitle.isEmpty()) sTitle = "Untitled";
	_CMyRichEdit* pRichEdit = new _CMyRichEdit(this);
	pRichEdit->setFrameShape(QFrame::NoFrame);
	pRichEdit->setContextMenuPolicy(Qt::CustomContextMenu);
	if(!sFilePath.isEmpty() && xFI.exists()) pRichEdit->open(sFilePath);
	m_pTabDocuments->addTab(pRichEdit, "");

	int nIndex = m_pTabDocuments->indexOf(pRichEdit);
	updateTabTitle(nIndex);
	updateWindowTitle();
	if(bFirst){
		onSwitchTab(0);
	}else{
		m_pTabDocuments->setCurrentIndex(nIndex);
	}
}

void MainWindow::updateEditActionsState()
{
	bool bHasEditor = m_pCurrentEdit;
	m_pActionSave->setEnabled(bHasEditor && m_pCurrentEdit->isDirty());
	m_pActionExportAsHtml->setEnabled(bHasEditor);
	m_pActionExportAsPdf->setEnabled(bHasEditor);
	m_pActionExportAsText->setEnabled(bHasEditor);
	m_pActionUndo->setEnabled(bHasEditor && m_pCurrentEdit->isUndoAvailable());
	m_pActionRedo->setEnabled(bHasEditor && m_pCurrentEdit->isRedoAvailable());
	m_pActionCut->setEnabled(bHasEditor);
	m_pActionCopy->setEnabled(bHasEditor);
	m_pActionPaste->setEnabled(bHasEditor);
	m_pActionFindText->setEnabled(bHasEditor);
	m_pActionReplaceText->setEnabled(bHasEditor);
	m_pActionBold->setEnabled(bHasEditor);
	m_pActionItalic->setEnabled(bHasEditor);
	m_pActionUnderline->setEnabled(bHasEditor);
	m_pActionOverline->setEnabled(bHasEditor);
	m_pActionStrikeOut->setEnabled(bHasEditor);
	m_pActionSubscript->setEnabled(bHasEditor);
	m_pActionSuperscript->setEnabled(bHasEditor);
	m_pComboTextFamily->setEnabled(bHasEditor);
	m_pComboTextSize->setEnabled(bHasEditor);
	m_pActionTextForeColor->setEnabled(bHasEditor);
	m_pActionTextBackColor->setEnabled(bHasEditor);
	m_pBtnTextForeColor->setEnabled(bHasEditor);
	m_pBtnTextBackColor->setEnabled(bHasEditor);
	m_pActionJustifyLeft->setEnabled(bHasEditor);
	m_pActionJustifyCenter->setEnabled(bHasEditor);
	m_pActionJustifyRight->setEnabled(bHasEditor);
	m_pActionParaMargin->setEnabled(bHasEditor);
	m_pActionIndent->setEnabled(bHasEditor);
	m_pActionOutdent->setEnabled(bHasEditor);
	m_pActionLineSpacingx1->setEnabled(bHasEditor);
	m_pActionLineSpacingx15->setEnabled(bHasEditor);
	m_pActionLineSpacingx2->setEnabled(bHasEditor);
	m_pActionInsOrderedList->setEnabled(bHasEditor);
	m_pActionInsUnorderedList->setEnabled(bHasEditor);
	m_pActionGrpBlockList->setEnabled(bHasEditor);
	m_pActionInsImage->setEnabled(bHasEditor);
	m_pActionExportImage->setEnabled(bHasEditor);
	m_pActionRotateImage->setEnabled(bHasEditor);
	m_pActionScaleImage->setEnabled(bHasEditor);
	m_pActionInsTable->setEnabled(bHasEditor);
	m_pActionInsRowBefore->setEnabled(bHasEditor);
	m_pActionInsRowAfter->setEnabled(bHasEditor);
	m_pActionInsColBefore->setEnabled(bHasEditor);
	m_pActionInsColAfter->setEnabled(bHasEditor);
	m_pActionRemoveRows->setEnabled(bHasEditor);
	m_pActionRemoveCols->setEnabled(bHasEditor);
	m_pActionMergeCells->setEnabled(bHasEditor);
	m_pActionSplitCells->setEnabled(bHasEditor);
	m_pActionTableBorderColor->setEnabled(bHasEditor);
	m_pActionCellBackColor->setEnabled(bHasEditor);
#if !defined(Q_OS_MAC)
	m_pBtnTableBorderColor->setEnabled(bHasEditor);
	m_pBtnCellBackColor->setEnabled(bHasEditor);
#endif
	m_pActionWidenCols->setEnabled(bHasEditor);
	m_pActionNarrowCols->setEnabled(bHasEditor);
	m_pActionEditHyperlink->setEnabled(bHasEditor);
	m_pActionCancelHyperlink->setEnabled(bHasEditor);
	if(!bHasEditor && m_pPanelFindReplace->isVisible()) m_pPanelFindReplace->setVisible(false);

	QTextCharFormat xFmtChar = bHasEditor ? m_pCurrentEdit->currentCharFormat() : QTextCharFormat();
	QTextBlockFormat xFmtBlock = bHasEditor ? m_pCurrentEdit->currentBlockFormat() : QTextBlockFormat();
	QTextListFormat::Style iListStyle = bHasEditor ? m_pCurrentEdit->blockTextListStyle() : QTextListFormat::ListStyleUndefined;
	int nSelectionBegin = -1, nSelectionEnd = -1; if(bHasEditor) m_pCurrentEdit->selection(nSelectionBegin, nSelectionEnd);
	bool bImageSelected = (bHasEditor && m_pCurrentEdit->isImageSelected());
	int nRowFirst = -1, nRowCount = -1, nColFirst = -1, nColCount = -1; if(bHasEditor) m_pCurrentEdit->selectedTableCells(&nRowFirst, &nRowCount, &nColFirst, &nColCount);

	onCurrentCharFormatChanged(xFmtChar);
	onCurrentBlockFormatChanged(xFmtBlock, iListStyle);
	onImageSelectionChanged(bImageSelected);
	onCellsSelectionChanged(nRowFirst, nRowCount, nColFirst, nColCount);
	onTextSelectionChanged(nSelectionBegin, nSelectionEnd);
}

QString MainWindow::titleOfDocument(int nIndex)
{
	QString sTitle;
	_CMyRichEdit* pRichEdit = qobject_cast<_CMyRichEdit*>(m_pTabDocuments->widget(nIndex));
	if(pRichEdit){
		QString sFilePath = pRichEdit->filePath();
		sTitle = QFileInfo(sFilePath).baseName();
		if(sTitle.isEmpty()) sTitle = "Untitled";
		if(pRichEdit->isDirty()){
			sTitle += " *";
		}
	}
	return sTitle;
}

void MainWindow::updateWindowTitle()
{
	QString sTitle = titleOfDocument(m_pTabDocuments->currentIndex());
	if(!sTitle.isEmpty()) sTitle += " - ";
	sTitle += "Rich Text Editor";
	QMainWindow::setWindowTitle(sTitle);
}

void MainWindow::updateTabTitle(int nIndex)
{
	_CMyRichEdit* pRichEdit = qobject_cast<_CMyRichEdit*>(m_pTabDocuments->widget(nIndex));
	if(pRichEdit){
		QString sTitle = titleOfDocument(m_pTabDocuments->currentIndex());
		m_pTabDocuments->setTabText(nIndex, sTitle);
		m_pTabDocuments->setTabToolTip(nIndex, pRichEdit->filePath());
	}
}

bool MainWindow::saveFile(int nIndex)
{
	bool bSucc = false;
	_CMyRichEdit* pRichEdit = qobject_cast<_CMyRichEdit*>(m_pTabDocuments->widget(nIndex));
	if(pRichEdit && pRichEdit->isDirty()){
		QString sFilePath = pRichEdit->filePath();
		if(sFilePath.isEmpty() || !QFileInfo(sFilePath).exists()){
			QString sFn = QFileDialog::getSaveFileName(
				this
				, "Save document"
				, g_xOpt.m_sPathToOpenFile.isEmpty() ? QDir::homePath() : g_xOpt.m_sPathToOpenFile
				, "Extended Rich Text (*.htmlx);;All files (*.*)"
				, NULL
                , fdo
			);
			if(!sFn.isEmpty()){
				pRichEdit->setFilePath(sFn);
				pRichEdit->save();

				g_xOpt.m_sPathToOpenFile = QFileInfo(sFn).dir().absolutePath();
				bSucc = true;
			}
		}else{
			pRichEdit->save();
			bSucc = true;
		}
	}
	return bSucc;
}

bool MainWindow::confirmSaveFile(int nIndex)
{
	bool bOK = true;
	_CMyRichEdit* pRichEdit = qobject_cast<_CMyRichEdit*>(m_pTabDocuments->widget(nIndex));
	if(pRichEdit){
		if(pRichEdit->isDirty()){
			QString sTitle = QFileInfo(pRichEdit->filePath()).fileName(); if(sTitle.isEmpty()) sTitle = "Untitled";
			QString sMsg = "The document has been modified; Do you want to save the modifications?\n\n" + sTitle;
			QMessageBox::StandardButtons btns = (QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			int ans = QMessageBox::question(this, "Save Changes", sMsg, btns);
			if(ans == QMessageBox::Yes){
				bOK = saveFile(nIndex);
			}else if(ans == QMessageBox::No){
				bOK = true;
			}else if(ans == QMessageBox::Cancel){
				bOK = false;
			}
		}
	}
	return bOK;
}

//bool MainWindow::confirmSaveFiles()
//{
//	QList<_CMyRichEdit*> vMods;
//	for(int i = 0; i < m_pTabDocuments->count(); i++){
//		_CMyRichEdit* pRichEdit = qobject_cast<_CMyRichEdit*>(m_pTabDocuments->widget(i));
//		if(pRichEdit && pRichEdit->isDirty()){
//			vMods << pRichEdit;
//		}
//	}

//	if(!vMods.isEmpty()){
//		_CTextSplitter v("", "\n");
//		v << "The following document(s) have been modified; Do you want to save the modifications?";
//		v << "";
//		Q_FOREACH(_CMyRichEdit* pMod, vMods){
//			QString sTitle = QFileInfo(pMod->filePath()).fileName();
//			if(sTitle.isEmpty()) sTitle = "New Document";
//			v << sTitle;
//		}
//		int ans = QMessageBox::question(this, "Save Changes", (QString)v);
//		if(ans == QMessageBox::Yes){
//			for(){
//				bOK = saveFile(nIndex);
//			}
//		}else if(ans == QMessageBox::No){
//			bOK = true;
//		}else if(ans == QMessageBox::Cancel){
//			bOK = false;
//		}
//	}
//}

void MainWindow::onSwitchTab(int index)
{
	_CMyRichEdit* pRichEdit = qobject_cast<_CMyRichEdit*>(m_pTabDocuments->widget(index));
	if(pRichEdit){
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(undoAvailable(bool)), this, SLOT(onUndoAvailable(bool)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(redoAvailable(bool)), this, SLOT(onRedoAvailable(bool)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(onCurrentCharFormatChanged(QTextCharFormat)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(currentBlockFormatChanged(QTextBlockFormat, QTextListFormat::Style)), this, SLOT(onCurrentBlockFormatChanged(QTextBlockFormat, QTextListFormat::Style)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(imageSelectionChanged(bool)), this, SLOT(onImageSelectionChanged(bool)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(cellsSelectionChanged(int,int,int,int)), this, SLOT(onCellsSelectionChanged(int,int,int,int)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onUpdateEditorInfo()));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(contentsChange(int,int,int)), this, SLOT(onUpdateEditorInfo()));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(selectionChanged(int,int)), this, SLOT(onTextSelectionChanged(int,int)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequest(QPoint)));
		makeUniqueConnection(m_pCurrentEdit, pRichEdit, SIGNAL(linkHovered(QString)), this, SLOT(onHyperlinkHovered(QString)));
		makeConnections(m_pCurrentEdit, pRichEdit);
		m_pCurrentEdit = pRichEdit;

		updateEditActionsState();
		onModificationChanged(m_pCurrentEdit->isDirty());
		onUpdateEditorInfo();
	}
}

void MainWindow::onTabCloseRequested(int index)
{
	if(confirmSaveFile(index)){
		if(index == m_pTabDocuments->currentIndex()){
			m_pCurrentEdit = NULL;

			updateEditActionsState();
			onModificationChanged(false);
			onUpdateEditorInfo();
		}
		m_pTabDocuments->removeTab(index);
		updateWindowTitle();
	}
}

void MainWindow::onUpdateEditorInfo()
{
	if(m_pCurrentEdit){
		QString sText;
		if(m_pCurrentEdit->isImageSelected()){
			int w = m_pCurrentEdit->imageWidth();
			int h = m_pCurrentEdit->imageHeight();
			int rotation = m_pCurrentEdit->imageRotation();
			sText = QString("size %1 * %2, rotation %3")
				.arg(w).arg(h).arg(rotation);
		}else if(m_pCurrentEdit->isTableFocused()){
			int rowFirst = -1, colFirst = -1, rowCount = -1, colCount = -1;
			m_pCurrentEdit->selectedTableCells(&rowFirst, &rowCount, &colFirst, &colCount);
			sText = QString("(row %1, column %2), selection %3 * %4")
				.arg(rowFirst + 1).arg(colFirst + 1).arg(rowCount).arg(colCount);
		}else{
			int row = m_pCurrentEdit->textCursor().blockNumber() + 1;
			int col = m_pCurrentEdit->textCursor().columnNumber() + 1;
			int len = m_pCurrentEdit->length();
			sText = QString("block %1, column %2, length %3").arg(row).arg(col).arg(len);
		}
		m_pDisplayExtraInfo->setText(sText);
		if(!m_pDisplayExtraInfo->isVisible()) m_pDisplayExtraInfo->setVisible(true);
	}else{
		if(m_pDisplayExtraInfo->isVisible()) m_pDisplayExtraInfo->setVisible(false);
	}
}

void MainWindow::onLoadRecentFiles()
{
	const QStringList& vMRU = g_xOpt.m_vMruFiles;
	Q_FOREACH(QString sFn, vMRU){
		createEdit(sFn);
	}
}

void MainWindow::onNewFile()
{
	createEdit("");
}

void MainWindow::onOpenFile()
{
	QStringList vFiles = QFileDialog::getOpenFileNames(
			this
			, "Open rich text document"
			, g_xOpt.m_sPathToOpenFile.isEmpty() ? QDir::homePath() : g_xOpt.m_sPathToOpenFile
			, "Extended Rich Text (*.htmlx);;Hypertext (*.html *.htm);;All files (*.*)"
			, NULL
            , fdo
	);
	Q_FOREACH(QString sFilePath, vFiles){
		g_xOpt.m_sPathToOpenFile = QFileInfo(sFilePath).dir().absolutePath();
		createEdit(sFilePath);
	}
}

void MainWindow::onSaveFile()
{
	saveFile(m_pTabDocuments->currentIndex());
}

void MainWindow::onExportAsHtml()
{
	if(m_pCurrentEdit){
		QString sFn = QFileDialog::getSaveFileName(
			this
			, "Export as HTML document"
			, g_xOpt.m_sPathToExportFile.isEmpty() ? QDir::homePath() : g_xOpt.m_sPathToExportFile
			, "Hypertext (*.html *.htm);;All files (*.*)"
			, NULL
            , fdo
		);
		if(!sFn.isEmpty()){
			m_pCurrentEdit->exportAsHtml(sFn);
			g_xOpt.m_sPathToExportFile = QFileInfo(sFn).dir().absolutePath();
		}
	}
}

void MainWindow::onExportAsPdf()
{
	if(m_pCurrentEdit){
		QString sFn = QFileDialog::getSaveFileName(
			this
			, "Export as PDF document"
			, g_xOpt.m_sPathToExportFile.isEmpty() ? QDir::homePath() : g_xOpt.m_sPathToExportFile
			, "Portable Document File (*.pdf);;All files (*.*)"
			, NULL
            , fdo
		);
		if(!sFn.isEmpty()){
			m_pCurrentEdit->exportAsPdf(sFn);
			g_xOpt.m_sPathToExportFile = QFileInfo(sFn).dir().absolutePath();
		}
	}
}

void MainWindow::onExportAsText()
{
	if(m_pCurrentEdit){
		QString sFn = QFileDialog::getSaveFileName(
			this
			, "Export as plain text document"
			, g_xOpt.m_sPathToExportFile.isEmpty() ? QDir::homePath() : g_xOpt.m_sPathToExportFile
			, "Plain Text Document (*.txt);;All files (*.*)"
			, NULL
            , fdo
		);
		if(!sFn.isEmpty()){
			m_pCurrentEdit->exportAsPlainText(sFn);
			g_xOpt.m_sPathToExportFile = QFileInfo(sFn).dir().absolutePath();
		}
	}
}

void MainWindow::onFindText()
{
	if(!m_pPanelFindReplace->isVisible()) m_pPanelFindReplace->setVisible(true);
	if(m_pCurrentEdit && m_pCurrentEdit->hasSelection()){
		m_pPanelFindReplace->setFindString(m_pCurrentEdit->selectedText());
	}
	m_pPanelFindReplace->setFocusOnFindInput();
}

void MainWindow::onReplaceText()
{
	if(!m_pPanelFindReplace->isVisible()) m_pPanelFindReplace->setVisible(true);
	if(m_pCurrentEdit && m_pCurrentEdit->hasSelection()){
		m_pPanelFindReplace->setFindString(m_pCurrentEdit->selectedText());
		m_pPanelFindReplace->setFocusOnReplaceInput();
	}else{
		m_pPanelFindReplace->setFocusOnFindInput();
	}
}

void MainWindow::onFindPanelVisibilityChanged(bool bVisible)
{
	if(!bVisible && m_pCurrentEdit) m_pCurrentEdit->setFocus();
}

void MainWindow::onCustomFont()
{
	if(m_pCurrentEdit){
		QFontDialog dlg(m_pCurrentEdit->currentFont(), this);
		dlg.setWindowTitle("Select font for the currently selected text");
		if(dlg.exec() == QFontDialog::Accepted){
			QFont ftSelected = dlg.currentFont();
			m_pCurrentEdit->setCurrentFont(ftSelected);
		}
	}
}

void MainWindow::onUnderline()
{
	if(m_pCurrentEdit){
		if(!m_pCurrentEdit->fontUnderline()){
			m_pCurrentEdit->setFontUnderline(true);
			m_pCurrentEdit->setFontUnderlineColor(m_pMenuUnderline->underlineColor());
			m_pCurrentEdit->setFontUnderlineStyle(m_pMenuUnderline->underlineStyle());
		}else{
			m_pCurrentEdit->setFontUnderline(false);
		}
	}
}

void MainWindow::onSetUnderlineColor(const QColor& clColor)
{
	if(m_pCurrentEdit) m_pCurrentEdit->setFontUnderlineColor(clColor);
}

void MainWindow::onSetUnderlineStyle(QTextCharFormat::UnderlineStyle iStyle)
{
	if(m_pCurrentEdit) m_pCurrentEdit->setFontUnderlineStyle(iStyle);
}

void MainWindow::onTextFamily(const QFont& ftFont)
{
	if(m_bTextFamilySetable && m_pCurrentEdit) m_pCurrentEdit->setFontFamily(ftFont.family());
}

//void MainWindow::onTextSize(int nIndex)
//{
//	if(m_bTextSizeSetable){
//		if(nIndex >= 0){
//			int nSize = m_pComboTextSize->itemData(nIndex).toInt();
//			m_pRichEdit->setFontPointSize(nSize);
//		}
//	}
//}

void MainWindow::onTextSize(const QString& sText)
{
	if(m_bTextSizeSetable && m_pCurrentEdit){
		int nSize = sText.toInt();
		if(nSize >= 9 && nSize <= 99) m_pCurrentEdit->setFontPointSize(nSize);
	}
}

void MainWindow::onParaMargin()
{
	if(m_pCurrentEdit){
		int nStartIndent = m_pCurrentEdit->blockTextIndent();
		int nMarginLeft = m_pCurrentEdit->blockLeftMargin();
		int nMarginRight = m_pCurrentEdit->blockRightMargin();
		int nMarginTop = m_pCurrentEdit->blockTopMargin();
		int nMarginBottom = m_pCurrentEdit->blockBottomMargin();
		QDlgParaMargin dlg(nStartIndent, nMarginLeft, nMarginRight, nMarginTop, nMarginBottom, this);
		if(dlg.exec()){
			m_pCurrentEdit->beginEdit();
			m_pCurrentEdit->setBlockTextIndent(dlg.m_nStartIndent);
			m_pCurrentEdit->setBlockLeftMargin(dlg.m_nMarginLeft);
			m_pCurrentEdit->setBlockRightMargin(dlg.m_nMarginRight);
			m_pCurrentEdit->setBlockTopMargin(dlg.m_nMarginTop);
			m_pCurrentEdit->setBlockBottomMargin(dlg.m_nMarginBottom);
			m_pCurrentEdit->endEdit();
		}
	}
}

void MainWindow::onSetListStyle()
{
	QAction* pAction = qobject_cast<QAction*>(QObject::sender());
	if(m_pCurrentEdit && pAction){
		QTextListFormat::Style iStyle = (QTextListFormat::Style)pAction->data().toInt();
		m_pCurrentEdit->setBlockTextList(iStyle);
	}
}

void MainWindow::onInsertImage()
{
	if(m_pCurrentEdit){
		QStringList vFiles = QFileDialog::getOpenFileNames(
						 this
						 , "Select pictures to insert into the document"
						 , g_xOpt.m_sPathToInsImage.isEmpty() ? QDir::homePath() : g_xOpt.m_sPathToInsImage
						 , "Images (*.png *.bmp *.jpeg *.jpg);;All files (*.*)"
						 , NULL
                         , fdo
						 );
		if(!vFiles.isEmpty()){
			g_xOpt.m_sPathToInsImage = QFileInfo(vFiles[0]).dir().absolutePath();
			Q_FOREACH(QString sFilePath, vFiles){
				m_pCurrentEdit->insertImage(QPixmap(sFilePath));
			}
		}
	}
}

void MainWindow::onRotateImage()
{
	if(m_pCurrentEdit) m_pCurrentEdit->rotateImage(m_pCurrentEdit->imageRotation() + 30);
}

void MainWindow::onExportImage()
{
	if(m_pCurrentEdit){
		QString sFn = QFileDialog::getSaveFileName(
			this
			, "Export selected image"
			, g_xOpt.m_sPathToInsImage.isEmpty() ? QDir::homePath() : g_xOpt.m_sPathToInsImage
			, "Portable Network Graphics (*.png);;Joint Photographic Experts Group (*.jpg; *jpeg);;Bitmap (*.bmp);;All files (*.*)"
			, NULL
            , fdo
		);
		if(!sFn.isEmpty()){
			m_pCurrentEdit->exportSelectedImage(sFn);
			g_xOpt.m_sPathToInsImage = QFileInfo(sFn).dir().absolutePath();
		}
	}
}

void MainWindow::onScaleImage()
{
	if(m_pCurrentEdit){
		QDlgScaleImg dlg(m_pCurrentEdit->imageWidth(), m_pCurrentEdit->imageHeight(), this);
		if(dlg.exec()){
			int w = dlg.m_nWidth, h = dlg.m_nHeight;
			if(w >= 0 && h >= 0){
				m_pCurrentEdit->resizeImageTo(w, h);
			}
		}
	}
}

void MainWindow::onInsertTable()
{
	if(m_pCurrentEdit) m_pCurrentEdit->insertTable(3, 4);
}

void MainWindow::onWidenSelCols()
{
	if(m_pCurrentEdit) m_pCurrentEdit->increaseColsWidthConstraints(20);
}

void MainWindow::onNarrowSelCols()
{
	if(m_pCurrentEdit) m_pCurrentEdit->increaseColsWidthConstraints(-20);
}

void MainWindow::onEditHyperlink()
{
	if(m_pCurrentEdit){
		QInputDialog dlg(this);
		dlg.setWindowTitle("Edit Hyperlink");
		dlg.setWindowIcon(g_xOpt.getIconForToolBar("btn_hyperlink_edit"));
		dlg.setInputMode(QInputDialog::TextInput);
		dlg.setLabelText("Hyperlink to");
		dlg.setTextValue("http://");
		if(dlg.exec()){
			m_pCurrentEdit->setUrlForSelectedText(dlg.textValue());
		}
	}
}

void MainWindow::onCancelHyperlink()
{
	if(m_pCurrentEdit) m_pCurrentEdit->clearUrlForSelectedText();
}

void MainWindow::onViewGitHub()
{
	QDesktopServices::openUrl(QUrl("https://github.com/peihaowang/EasyEditor"));
}

void MainWindow::onAbout()
{
	QString sMsg = QString("<b style='font-size:20px;'>EasyEditor Beta-1</b>"
		"<br/>Copyright %1 Peihao Wang. All rights reserved."
		"<br/><br/><a href='https://github.com/peihaowang'>github.com/peihaowang</a>"
		"<br/><a href='mailto:wangpeihao@gmail.com'>mailto:wangpeihao@gmail.com</a>"
		"<br/><br/>A sophisticated editor for rich text editing and formatting, extended from HTML.")
		.arg(QDate::currentDate().year() == 2018 ? "2018" : QString("2018 ~ %1").arg(QDate::currentDate().year()))
		;
	QMessageBox::about(this, "About EasyEditor", sMsg);
}

void MainWindow::onHyperlinkHovered(const QString& sUrl)
{
	QMainWindow::statusBar()->showMessage(sUrl);
}

void MainWindow::onHyperlinkClicked(const QString& sUrl)
{
	QDesktopServices::openUrl(sUrl);
}

void MainWindow::onUndoAvailable(bool bCan)
{
	m_pActionUndo->setEnabled(m_pCurrentEdit && bCan);
}

void MainWindow::onRedoAvailable(bool bCan)
{
	m_pActionRedo->setEnabled(m_pCurrentEdit && bCan);
}

void MainWindow::onModificationChanged(bool bModified)
{
	m_pActionSave->setEnabled(bModified);
	updateTabTitle(m_pTabDocuments->currentIndex());
	updateWindowTitle();
}

void MainWindow::onCurrentCharFormatChanged(const QTextCharFormat& xFmt)
{	
	m_pActionBold->setChecked(xFmt.fontWeight() == QFont::Bold);
	m_pActionItalic->setChecked(xFmt.fontItalic());
	m_pActionUnderline->setChecked(xFmt.underlineStyle() != QTextCharFormat::NoUnderline);
	m_pActionOverline->setChecked(xFmt.fontOverline());
	m_pActionStrikeOut->setChecked(xFmt.fontStrikeOut());
	m_pActionSubscript->setChecked(xFmt.verticalAlignment() == QTextCharFormat::AlignSubScript);
	m_pActionSuperscript->setChecked(xFmt.verticalAlignment() == QTextCharFormat::AlignSuperScript);

	{
		_CTempValueChange<bool> _tvc(m_bTextFamilySetable, false); Q_UNUSED(_tvc);
		m_pComboTextFamily->setCurrentFont(xFmt.font());
	}

	{
		_CTempValueChange<bool> _tvc(m_bTextSizeSetable, false); Q_UNUSED(_tvc);
		int nIndex = m_pComboTextSize->findData(QVariant(xFmt.fontPointSize()));
		if(nIndex >= 0){
			m_pComboTextSize->setCurrentIndex(nIndex);
		}else{
			m_pComboTextSize->setEditText(QString::number(xFmt.fontPointSize()));
		}
	}
}

void MainWindow::onCurrentBlockFormatChanged(const QTextBlockFormat& xFmt, QTextListFormat::Style iStyle)
{
	m_pActionJustifyLeft->setChecked(xFmt.alignment() & Qt::AlignLeft);
	m_pActionJustifyCenter->setChecked(xFmt.alignment() & Qt::AlignHCenter);
	m_pActionJustifyRight->setChecked(xFmt.alignment() & Qt::AlignRight);

	static QList<QTextListFormat::Style> vOrderedList, vUnorderedList;
	if(vOrderedList.isEmpty()){
		vOrderedList << QTextListFormat::ListDecimal;
		vOrderedList << QTextListFormat::ListLowerAlpha;
		vOrderedList << QTextListFormat::ListUpperAlpha;
		vOrderedList << QTextListFormat::ListLowerRoman;
		vOrderedList << QTextListFormat::ListUpperRoman;
	}
	if(vUnorderedList.isEmpty()){
		vUnorderedList << QTextListFormat::ListDisc;
		vUnorderedList << QTextListFormat::ListCircle;
		vUnorderedList << QTextListFormat::ListSquare;
	}
	m_pActionInsOrderedList->setChecked(vOrderedList.indexOf(iStyle) >= 0);
	m_pActionInsUnorderedList->setChecked(vUnorderedList.indexOf(iStyle) >= 0);
	{
		QList<QAction*> vActions = m_pActionGrpBlockList->actions();
		Q_FOREACH(QAction* pAction, vActions){
			pAction->setChecked(pAction->data().toInt() == iStyle);
		}
	}

	m_pActionLineSpacingx1->setChecked(m_pCurrentEdit && m_pCurrentEdit->isLineSpacingx1());
	m_pActionLineSpacingx15->setChecked(m_pCurrentEdit && m_pCurrentEdit->isLineSpacingx15());
	m_pActionLineSpacingx2->setChecked(m_pCurrentEdit && m_pCurrentEdit->isLineSpacingx2());
}

void MainWindow::onTextSelectionChanged(int nBegin, int nEnd)
{
	bool bSelected = (nBegin >= 0) && (nEnd >= 0) && (nBegin < nEnd);
	m_pActionEditHyperlink->setEnabled(bSelected);
	m_pActionCancelHyperlink->setEnabled(bSelected);
}

void MainWindow::onImageSelectionChanged(bool bSelected)
{
	m_pActionExportImage->setEnabled(bSelected);
	m_pActionRotateImage->setEnabled(bSelected);
	m_pActionScaleImage->setEnabled(bSelected);
}

void MainWindow::onCellsSelectionChanged(int nRowFirst, int nRowCount, int nColFirst, int nColCount)
{
	bool bTableFocused = (nRowFirst >= 0 && nColFirst >= 0);
	bool bCellsSelected = (nRowCount > 1 || nColCount > 1);

	m_pActionInsRowBefore->setEnabled(bTableFocused);
	m_pActionInsRowAfter->setEnabled(bTableFocused);
	m_pActionInsColBefore->setEnabled(bTableFocused);
	m_pActionInsColAfter->setEnabled(bTableFocused);
	m_pActionRemoveRows->setEnabled(bTableFocused);
	m_pActionRemoveCols->setEnabled(bTableFocused);
	m_pActionMergeCells->setEnabled(bCellsSelected);
	m_pActionSplitCells->setEnabled(bTableFocused);
	m_pActionTableBorderColor->setEnabled(bTableFocused);
	m_pActionCellBackColor->setEnabled(bTableFocused);
#if !defined(Q_OS_MAC)
	m_pBtnTableBorderColor->setEnabled(bTableFocused);
	m_pBtnCellBackColor->setEnabled(bTableFocused);
#endif
	m_pActionWidenCols->setEnabled(bTableFocused);
	m_pActionNarrowCols->setEnabled(bTableFocused);
}

void MainWindow::onContextMenuRequest(const QPoint& pos)
{
	_CMyRichEdit* pSender = qobject_cast<_CMyRichEdit*>(QObject::sender());
	if(pSender){
		QMenu xMenuCxt(qobject_cast<QWidget*>(QObject::sender()));
		xMenuCxt.addAction(m_pActionUndo);
		xMenuCxt.addAction(m_pActionRedo);
		xMenuCxt.addSeparator();
		xMenuCxt.addAction(m_pActionCut);
		xMenuCxt.addAction(m_pActionCopy);
		xMenuCxt.addAction(m_pActionPaste);
		xMenuCxt.addSeparator();
		xMenuCxt.addAction(m_pActionFindText);
		xMenuCxt.addAction(m_pActionReplaceText);
		xMenuCxt.addSeparator();
		xMenuCxt.addAction(m_pActionBold);
		xMenuCxt.addAction(m_pActionItalic);
		xMenuCxt.addAction(m_pActionUnderline);
		xMenuCxt.addAction(m_pActionOverline);
		xMenuCxt.addAction(m_pActionSubscript);
		xMenuCxt.addAction(m_pActionSuperscript);
		xMenuCxt.addSeparator();
		xMenuCxt.addAction(m_pActionEditHyperlink);
		xMenuCxt.addAction(m_pActionCancelHyperlink);

		xMenuCxt.exec(pSender->mapToGlobal(pos));
	}
}
