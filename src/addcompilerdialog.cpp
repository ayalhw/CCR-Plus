#include "addcompilerdialog.h"
#include "ui_addcompilerdialog.h"

#include <QDebug>

AddCompilerDialog::AddCompilerDialog(Problem* problem, Compiler* compiler, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddCompilerDialog), problem(problem), compiler(compiler)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedHeight(this->sizeHint().height());

    if (!compiler)
        this->setWindowTitle("添加编译器 - " + problem->Name());
    else
        this->setWindowTitle("编辑编译器 - " + problem->Name());

    initCompiler(compiler);
}

AddCompilerDialog::~AddCompilerDialog()
{
    delete ui;
}

void AddCompilerDialog::accept()
{
    file = ui->lineEdit_file->text();
    cmd = ui->lineEdit_cmd->text();
    time_lim = ui->spinBox_timeLim->value();

    if (file.isEmpty())
    {
        ui->label_error->setText("源程序文件名为空。");
        ui->lineEdit_file->setFocus();
        return;
    }
    switch (ui->comboBox_type->currentIndex())
    {
    case 0: // C
        if (!file.endsWith(".c"))
        {
            ui->label_error->setText("源程序扩展名要求为\".c\"");
            ui->lineEdit_file->setFocus();
            return;
        }
        break;
    case 1: // C++
        if (!file.endsWith(".cpp"))
        {
            ui->label_error->setText("源程序扩展名要求为\".cpp\"");
            ui->lineEdit_file->setFocus();
            return;
        }
        break;
    case 2: // Pascal
        if (!file.endsWith(".pas"))
        {
            ui->label_error->setText("源程序扩展名要求为\".pas\"");
            ui->lineEdit_file->setFocus();
            return;
        }
        break;
    }
    if (!ui->label_error->text().isEmpty()) return;
    if (compiler) *compiler = Compiler(cmd, file, time_lim);
    QDialog::accept();
}


void AddCompilerDialog::initCompiler(Compiler *compiler)
{
    if (!compiler)
    {
        ui->comboBox_type->setCurrentIndex(0);
        return;
    }

    file = compiler->SourceFile();
    cmd = compiler->Cmd();
    time_lim = compiler->TimeLimit();
    ui->spinBox_timeLim->setValue(time_lim);

    auto format = [](QString str)
    {
        if (str.startsWith("\'") && str.endsWith("\'")) str = str.mid(1, str.length() - 2);
        if (str.startsWith("\"") && str.endsWith("\"")) str = str.mid(1, str.length() - 2);
        return str;
    };

    bool ok = true;
    if (file.endsWith(".c"))
    {
        ui->comboBox_type->setCurrentIndex(0);
        QStringList list = cmd.simplified().split(" "), args;
        if (list[0] != "gcc") ok = false;
        for (int i = 0; i < list.size(); i++)
        {
            QString str = format(list[i]);
            if (str == "-o") i++;
            else if (!str.isEmpty() && str != "gcc" && str != file) args.append(list[i]);
        }
        ui->lineEdit_args->setText(args.join(" "));
    }
    else if (file.endsWith(".cpp"))
    {
        ui->comboBox_type->setCurrentIndex(1);
        QStringList list = cmd.simplified().split(" "), args;
        if (list[0] != "g++") ok = false;
        for (int i = 0; i < list.size(); i++)
        {
            QString str = format(list[i]);
            if (str == "-o") i++;
            else if (!str.isEmpty() && str != "g++" && str != file) args.append(list[i]);
        }
        ui->lineEdit_args->setText(args.join(" "));
    }
    else if (file.endsWith(".pas"))
    {
        ui->comboBox_type->setCurrentIndex(2);
        QStringList list = cmd.simplified().split(" "), args;
        if (list[0] != "fpc") ok = false;
        for (auto i : list)
        {
            QString str = format(i);
            if (!str.isEmpty() && str != "fpc" && str != file) args.append(i);
        }
        ui->lineEdit_args->setText(args.join(" "));
    }
    else
        ui->comboBox_type->setCurrentIndex(3);

    ui->lineEdit_file->setText(file);
    ui->lineEdit_cmd->setText(cmd);
    if (!ok) ui->label_error->setText("无效的编译命令。");
}

void AddCompilerDialog::onChangeCmd()
{
    QString file = ui->lineEdit_file->text();
    QString args = ui->lineEdit_args->text();
    QString exe = Problem::RemoveFileExtension(problem->ExecutableFile());
    switch (ui->comboBox_type->currentIndex())
    {
    case 0: // C
        ui->lineEdit_cmd->setText(QString("gcc -o %1 %2 %3").arg(exe).arg(file).arg(args));
        break;
    case 1: // C++
        ui->lineEdit_cmd->setText(QString("g++ -o %1 %2 %3").arg(exe).arg(file).arg(args));
        break;
    case 2: // Pascal
        ui->lineEdit_cmd->setText(QString("fpc %1 %2").arg(file).arg(args));
        break;
    }
}

void AddCompilerDialog::on_comboBox_type_currentIndexChanged(int index)
{
    ui->label_error->setText("");
    switch (index)
    {
    case 0: // C
        ui->lineEdit_file->setText(problem->Name() + ".c");
        ui->lineEdit_args->setText("-lm -static");
        ui->lineEdit_cmd->setReadOnly(true);
        ui->label_4->setEnabled(true);
        ui->lineEdit_args->setEnabled(true);
        break;
    case 1: // C++
        ui->lineEdit_file->setText(problem->Name() + ".cpp");
        ui->lineEdit_args->setText("-lm -static");
        ui->lineEdit_cmd->setReadOnly(true);
        ui->label_4->setEnabled(true);
        ui->lineEdit_args->setEnabled(true);
        break;
    case 2: // Pascal
        ui->lineEdit_file->setText(problem->Name() + ".pas");
        ui->lineEdit_args->setText("");
        ui->lineEdit_cmd->setReadOnly(true);
        ui->label_4->setEnabled(true);
        ui->lineEdit_args->setEnabled(true);
        break;
    case 3: // Other
        ui->lineEdit_file->setText("");
        ui->lineEdit_args->setText("");
        ui->lineEdit_cmd->setText("");
        ui->lineEdit_cmd->setReadOnly(false);
        ui->label_4->setEnabled(false);
        ui->lineEdit_args->setEnabled(false);
        break;
    }
}

void AddCompilerDialog::on_lineEdit_file_textChanged(const QString& /*arg1*/)
{
    onChangeCmd();
    ui->label_error->setText("");
}

void AddCompilerDialog::on_lineEdit_args_textChanged(const QString& /*arg1*/)
{
    onChangeCmd();
}
