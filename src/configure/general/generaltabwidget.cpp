#include <QDir>
#include <QListView>
#include <QStandardItemModel>

#include "common/global.h"
#include "common/problem.h"
#include "configure/general/generaltabwidget.h"
#include "ui_generaltabwidget.h"

GeneralTabWidget::GeneralTabWidget(QWidget* parent) :
    ConfigureTabWidget(parent),
    ui(new Ui::GeneralTabWidget), load_finished(false)
{
    ui->setupUi(this);

    QStandardItemModel* model;
    QStandardItem* item;

    model = new QStandardItemModel(ui->comboBox_builtin);
    auto& builtin_checker = Problem::BUILTIN_CHECKER_MAP;
    for (auto checker : builtin_checker)
    {
        item = new QStandardItem(checker.first);
        item->setToolTip(checker.second);
        model->appendRow(item);
    }
    ui->comboBox_builtin->setModel(model);
    ui->comboBox_builtin->setView(new QListView(ui->comboBox_builtin));
}

GeneralTabWidget::~GeneralTabWidget()
{
    delete ui;
}

void GeneralTabWidget::ShowProblemConfiguration(Problem* problem)
{
    load_finished = false;
    current_problem = problem;

    ui->lineEdit_dir->setText(problem->Directory());
    ui->spinBox_codeLim->setValue(problem->CodeLengthLimit());

    if (problem->Type() == Global::Traditional)
    {
        ui->lineEdit_exe->setText(Problem::RemoveFileExtension(problem->ExecutableFile()));
        ui->lineEdit_inFile->setText(problem->InFile());
        ui->lineEdit_outFile->setText(problem->OutFile());
        ui->spinBox_codeLim->setEnabled(true);
        ui->groupBox_run->setEnabled(true);
    }
    else if (problem->Type() == Global::AnswersOnly)
    {
        ui->lineEdit_exe->clear();
        ui->lineEdit_inFile->clear();
        ui->lineEdit_outFile->clear();
        ui->spinBox_codeLim->setEnabled(false);
        ui->groupBox_run->setEnabled(false);
    }


    ui->radioButton_custom->setStyleSheet("");
    ui->radioButton_custom->setToolTip("");
    ui->comboBox_custom->setToolTip("");

    ui->comboBox_custom->clear();
    ui->comboBox_builtin->setCurrentIndex(0);
    QStandardItemModel* model = new QStandardItemModel(ui->comboBox_custom);
    QStringList dirs = { QCoreApplication::applicationDirPath() + "/checker",
                         Global::g_contest.data_path + problem->Name(),
                       };
    for (auto dir : dirs)
    {
#ifdef Q_OS_WIN
        QStringList list = QDir(dir).entryList(QDir::Files);
#else
        QStringList list = QDir(dir).entryList(QDir::Files | QDir::Executable);
#endif
        for (auto checker : list)
        {
#ifdef Q_OS_WIN
            if (!checker.endsWith(".exe")) continue;
#endif
            if (Problem::IsBuiltinChecker(checker)) continue;

            QStandardItem* item = new QStandardItem(checker);
            item->setToolTip(QString("%1 (位置: %2)").arg(checker, dir));
            model->appendRow(item);
        }
    }
    ui->comboBox_custom->setModel(model);
    ui->comboBox_custom->setView(new QListView(ui->comboBox_custom));

    if (Problem::IsBuiltinChecker(problem->Checker()))
    {
        ui->radioButton_builtin->setChecked(true);
        ui->radioButton_custom->setChecked(false);
        ui->comboBox_builtin->setEnabled(true);
        ui->comboBox_custom->setEnabled(false);
        ui->comboBox_builtin->setCurrentText(problem->BuiltinCheckerName());
    }
    else
    {
        ui->radioButton_builtin->setChecked(false);
        ui->radioButton_custom->setChecked(true);
        ui->comboBox_builtin->setEnabled(false);
        ui->comboBox_custom->setEnabled(true);
        ui->comboBox_custom->setCurrentText(problem->Checker());

        if (ui->comboBox_custom->currentText().isEmpty())
        {
            ui->radioButton_custom->setStyleSheet("QRadioButton{color:red;}");
            ui->radioButton_custom->setToolTip("找不到自定义校验器。");
            ui->comboBox_custom->setToolTip("找不到自定义校验器。");
        }
    }
    ui->spinBox_checkerTimeLim->setValue(problem->CheckerTimeLimit());

    load_finished = true;
}

void GeneralTabWidget::ChangeProblemType(Global::ProblemType type)
{
    ui->spinBox_codeLim->setValue(current_problem->CodeLengthLimit());

    if (type == Global::Traditional)
    {
        ui->lineEdit_exe->setText(Problem::RemoveFileExtension(current_problem->ExecutableFile()));
        ui->lineEdit_inFile->setText(current_problem->InFile());
        ui->lineEdit_outFile->setText(current_problem->OutFile());
        ui->spinBox_codeLim->setEnabled(true);
        ui->groupBox_run->setEnabled(true);
    }
    else if (type == Global::AnswersOnly)
    {
        ui->lineEdit_exe->clear();
        ui->lineEdit_inFile->clear();
        ui->lineEdit_outFile->clear();
        ui->spinBox_codeLim->setEnabled(false);
        ui->groupBox_run->setEnabled(false);
    }
}

void GeneralTabWidget::Reset()
{
    on_pushButton_resetSubmit_clicked();
    on_pushButton_resetRun_clicked();
    on_pushButton_resetChecker_clicked();
}

void GeneralTabWidget::FocusErrorLine()
{
    if (!ui->lineEdit_dir->styleSheet().isEmpty()) ui->lineEdit_dir->setFocus();
    else if (!ui->lineEdit_exe->styleSheet().isEmpty()) ui->lineEdit_exe->setFocus();
    else if (!ui->lineEdit_inFile->styleSheet().isEmpty()) ui->lineEdit_inFile->setFocus();
    else if (!ui->lineEdit_outFile->styleSheet().isEmpty()) ui->lineEdit_outFile->setFocus();
    else if (!ui->radioButton_custom->styleSheet().isEmpty()) ui->radioButton_custom->setFocus();
}



void GeneralTabWidget::on_radioButton_builtin_clicked()
{
    ui->radioButton_builtin->setChecked(true);
    ui->radioButton_custom->setChecked(false);
    ui->comboBox_builtin->setEnabled(true);
    ui->comboBox_custom->setEnabled(false);
    ui->comboBox_builtin->setCurrentIndex(0);
    on_comboBox_builtin_currentIndexChanged(ui->comboBox_builtin->currentText());

    ui->radioButton_custom->setStyleSheet("");
    ui->radioButton_custom->setToolTip("");
    ui->comboBox_custom->setToolTip("");
}

void GeneralTabWidget::on_radioButton_custom_clicked()
{
    ui->radioButton_builtin->setChecked(false);
    ui->radioButton_custom->setChecked(true);
    ui->comboBox_builtin->setEnabled(false);
    ui->comboBox_custom->setEnabled(true);
    ui->comboBox_custom->setCurrentIndex(0);
    on_comboBox_custom_currentIndexChanged(ui->comboBox_custom->currentText());

    if (ui->comboBox_custom->currentText().isEmpty())
    {
        ui->radioButton_custom->setStyleSheet("QRadioButton{color:red;}");
        ui->radioButton_custom->setToolTip("找不到自定义校验器。");
        ui->comboBox_custom->setToolTip("找不到自定义校验器。");
    }
    else
    {
        ui->radioButton_custom->setStyleSheet("");
        ui->radioButton_custom->setToolTip("");
        ui->comboBox_custom->setToolTip("");
    }
}

void GeneralTabWidget::on_pushButton_resetSubmit_clicked()
{
    ui->lineEdit_dir->setText(current_problem->Name());
    ui->spinBox_codeLim->setValue(100);
}

void GeneralTabWidget::on_pushButton_resetRun_clicked()
{
    if (current_problem->Type() == Global::Traditional)
    {
        ui->lineEdit_exe->setText(current_problem->Name());
        ui->lineEdit_inFile->setText(current_problem->Name() + ".in");
        ui->lineEdit_outFile->setText(current_problem->Name() + ".out");
    }
    else if (current_problem->Type() == Global::AnswersOnly)
    {
        ui->lineEdit_exe->clear();
        ui->lineEdit_inFile->clear();
        ui->lineEdit_outFile->clear();
    }
}

void GeneralTabWidget::on_pushButton_resetChecker_clicked()
{
    on_radioButton_builtin_clicked();
    ui->spinBox_checkerTimeLim->setValue(10);
}

void GeneralTabWidget::on_lineEdit_dir_textChanged(const QString& text)
{
    QString msg = Problem::CheckFileNameValid(text);
    if (msg.isEmpty())
    {
        ui->lineEdit_dir->setStyleSheet("");
        ui->lineEdit_dir->setToolTip("");
    }
    else
    {
        ui->lineEdit_dir->setStyleSheet("QLineEdit{color:red;}");
        ui->lineEdit_dir->setToolTip(msg.arg("提交目录"));
    }
    if (load_finished) current_problem->SetDirectory(text.trimmed());
}

void GeneralTabWidget::on_spinBox_codeLim_valueChanged(double val)
{
    if (load_finished) current_problem->SetCodeLengthLimit(val);
}

void GeneralTabWidget::on_lineEdit_exe_textChanged(const QString& text)
{
    QString msg = Problem::CheckFileNameValid(text);
    if (text.contains(' ') || text.contains('\t'))
    {
        ui->lineEdit_exe->setStyleSheet("QLineEdit{color:red;}");
        ui->lineEdit_exe->setToolTip("可执行文件名不能包含空白字符。");
    }
    else if (msg.isEmpty())
    {
        ui->lineEdit_exe->setStyleSheet("");
        ui->lineEdit_exe->setToolTip("");
    }
    else
    {
        ui->lineEdit_exe->setStyleSheet("QLineEdit{color:red;}");
        ui->lineEdit_exe->setToolTip(msg.arg("可执行文件名"));
    }
    if (load_finished) current_problem->SetExecutableFile(text);
}

void GeneralTabWidget::on_lineEdit_inFile_textChanged(const QString& text)
{
    QString msg = Problem::CheckFileNameValid(text);
    if (msg.isEmpty())
    {
        ui->lineEdit_inFile->setStyleSheet("");
        ui->lineEdit_inFile->setToolTip("");
    }
    else
    {
        ui->lineEdit_inFile->setStyleSheet("QLineEdit{color:red;}");
        ui->lineEdit_inFile->setToolTip(msg.arg("输入文件名"));
    }
    if (load_finished) current_problem->SetInFile(text.trimmed());
}

void GeneralTabWidget::on_lineEdit_outFile_textChanged(const QString& text)
{
    QString msg = Problem::CheckFileNameValid(text);
    if (msg.isEmpty())
    {
        ui->lineEdit_outFile->setStyleSheet("");
        ui->lineEdit_outFile->setToolTip("");
    }
    else
    {
        ui->lineEdit_outFile->setStyleSheet("QLineEdit{color:red;}");
        ui->lineEdit_outFile->setToolTip(msg.arg("输出文件名"));
    }
    if (load_finished) current_problem->SetOutFile(text.trimmed());
}

void GeneralTabWidget::on_comboBox_builtin_currentIndexChanged(const QString& text)
{
    if (load_finished) current_problem->SetChecker(Problem::FromBuiltinCheckerName(text));
}

void GeneralTabWidget::on_comboBox_custom_currentIndexChanged(const QString& text)
{
    if (load_finished) current_problem->SetChecker(text);
}

void GeneralTabWidget::on_spinBox_checkerTimeLim_valueChanged(int val)
{
    if (load_finished) current_problem->SetCheckerTimeLimit(val);
}
