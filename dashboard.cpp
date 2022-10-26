#include "dashboard.h"
#include "ui_dashboard.h"
#include "QInputDialog.h"
#include "QMessageBox.h"
#include <windows.h>
#include <QLineEdit>
#include <QDebug>
#include "component.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//表格
dashboard::dashboard(bool power,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dashboard)
{
    ui->setupUi(this);
    this->setWindowTitle("机房失物招领");
    if(power){
        ui->confirmRecord->setDisabled(false);
        ui->found->setDisabled(false);
        ui->removeRecord->setDisabled(false);
        ui->removeAll->setDisabled(false);
    }
    ui->recordTable->setColumnCount(8);
    ui->recordTable->setColumnWidth(0,120);
    ui->recordTable->setColumnWidth(1,140);
    ui->recordTable->setColumnWidth(2,120);
    ui->recordTable->setColumnWidth(3,120);
    ui->recordTable->setColumnWidth(4,120);
    ui->recordTable->setColumnWidth(5,120);
    ui->recordTable->setColumnWidth(6,120);
    ui->recordTable->setColumnWidth(7,200);
    ui->recordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->recordTable->setSelectionMode ( QAbstractItemView::SingleSelection);
    ui->recordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->recordTable->verticalHeader()->setVisible(true);
    QStringList sListHeader;
    sListHeader<<"物品名称"<<"物品类别"<<"丢失时间"<<"丢失地点"<<"状态"<<"认领人姓名"<<"认领人手机号"<<"认领人学号"<<"认领时间";
    ui->recordTable->setHorizontalHeaderLabels(sListHeader);
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    this->update(allRecord);
    initFindDlg();
}




void dashboard::initFindDlg()
{
    findDlg = new QDialog(this);
    //新建一个对话框，用于查找操作。
    findDlg->setWindowTitle(tr("查找"));
    //设置对话框的标题
    find_textLineEdit = new QLineEdit(findDlg);
    //将行编辑器加入到新建的查找对话框中
    QPushButton *findbutton = new QPushButton(tr("查找下一个"),findDlg);
    //加入一个“查找下一个”的按钮
    QVBoxLayout* layout = new QVBoxLayout(findDlg);
    layout->addWidget(find_textLineEdit);
    layout->addWidget(findbutton);
    //新建一个垂直布局管理器，并将行编辑器和按钮加入其中

    //显示对话框
    connect(findbutton,SIGNAL(clicked()),this,SLOT(show_findText()));
    //设置“查找下一个”按钮的单击事件和其槽函数的关联
}


dashboard::~dashboard()
{
    delete ui;
}

// 更新表格
void dashboard::update(vector<lost> &allRecord){
    int length = allRecord.size();
    ui->recordTable->setRowCount(length);
    for(int i=0;i<length;i++){
        string name = allRecord[i].getName();
        string category = allRecord[i].getCategory();
        string lostTime = allRecord[i].getLostDay();
        string place = allRecord[i].getPlace();
        string state = allRecord[i].state2String();
        string personName =allRecord[i].getStuName();
        string personPhone =allRecord[i].getPhoneNumber();
        string personId =allRecord[i].getStuId();
        string pickUpTime =allRecord[i].getPickUpTime();
        ui->recordTable->setItem(i,0,new QTableWidgetItem(QString::fromStdString(name)));
        ui->recordTable->setItem(i,1,new QTableWidgetItem(QString::fromStdString(category)));
        ui->recordTable->setItem(i,2,new QTableWidgetItem(QString::fromStdString(lostTime)));
        ui->recordTable->setItem(i,3,new QTableWidgetItem(QString::fromStdString(place)));
        ui->recordTable->setItem(i,4,new QTableWidgetItem(QString::fromStdString(state)));
        ui->recordTable->setItem(i,5,new QTableWidgetItem(QString::fromStdString(personName)));
        ui->recordTable->setItem(i,6,new QTableWidgetItem(QString::fromStdString(personPhone)));
        ui->recordTable->setItem(i,7,new QTableWidgetItem(QString::fromStdString(personId)));
        ui->recordTable->setItem(i,8,new QTableWidgetItem(QString::fromStdString(pickUpTime)));
    }
}

//高亮单个行
int dashboard::getSelectedSingleRow(){
    QList<QTableWidgetSelectionRange> ranges =ui->recordTable->selectedRanges();
    int length = ranges.length();
    if(length!=0){
        return ranges[0].topRow();
    }
    else{
        return -1;
    }
}

//新建记录
void dashboard::on_newRecord_clicked()
{
    bool nameOk,placeOk,cateOK;
    string nameText = QInputDialog::getText(NULL, "新建记录","请输入物品名称",QLineEdit::Normal,"物品名称",&nameOk).toStdString().c_str();
    if(!nameOk) return;
    string placeText = QInputDialog::getText(NULL, "新建记录","请输入丢失地点",QLineEdit::Normal,"物品地点",&placeOk).toStdString().c_str();
    if(!placeOk) return;
    string cateText = QInputDialog::getText(NULL, "新建记录","请输入丢失物品类别",QLineEdit::Normal,"物品类别",&cateOK).toStdString().c_str();
    if(!cateOK) return;

    time_t t=time(0);
    tm *ltm = localtime(&t);
    lost a(nameText,cateText,placeText,ltm);
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    allRecord.push_back(a);
    Util::setStorageSync("record",allRecord);
    this->update(allRecord);
    QMessageBox::about(NULL, "创建成功", "记录创建成功");
}

//认领记录
void dashboard::on_found_clicked()
{
    //点击认领
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    if(allRecord[this->getSelectedSingleRow()].getState()!=2){
        QMessageBox::about(NULL, "错误", "需管理老师确认才可认领物品");
        return;
    }
    bool nameOk,phoneOk,idOk;
    string nameText = QInputDialog::getText(NULL, "认领物品","请输入认领人名称",QLineEdit::Normal,"认领人名称",&nameOk).toStdString();
    if(!nameOk) return;
    string phoneText = QInputDialog::getText(NULL, "认领物品","请输入认领人手机号",QLineEdit::Normal,"认领人手机号",&phoneOk).toStdString();
    if(!phoneOk) return;
    string idText = QInputDialog::getText(NULL, "认领物品","请输入认领人学号",QLineEdit::Normal,"认领人学号",&idOk).toStdString();
    if(!idOk) return;
    time_t t=time(0);
    tm *ltm = localtime(&t);
    allRecord[this->getSelectedSingleRow()].pickUp(nameText,phoneText,idText,*ltm);
    Util::setStorageSync<lost>("record",allRecord);
    this->update(allRecord);
    QMessageBox::about(NULL, "认领成功", "认领成功");
}

//确认记录
void dashboard::on_confirmRecord_clicked()
{
    //点击确认记录
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    allRecord[this->getSelectedSingleRow()].verify();
    Util::setStorageSync<lost>("record",allRecord);
    this->update(allRecord);
}

//删除记录（全部）
void dashboard::on_removeAll_clicked()
{
    vector<lost> allRecord;
    Util::setStorageSync<lost>("record",allRecord);
    this->update(allRecord);
    QMessageBox::about(NULL, "所有记录已删除", "所有记录已删除");
}

//删除单个记录
void dashboard::on_removeRecord_clicked()
{
    int index = this->getSelectedSingleRow();//选定单个行，以删除记录
    if(index!=-1){
        vector<lost> allRecord = Util::getStorageSync<lost>("record");
        allRecord.erase(allRecord.begin()+index);
        Util::setStorageSync<lost>("record",allRecord);
        this->update(allRecord);
        QMessageBox::about(NULL, "记录已删除", "记录已删除");
    }
    else{
        QMessageBox::about(NULL, "错误", "请选择一项记录");
    }
}

void dashboard::on_sortByLost_clicked()
{
    // 点击按丢失时间排序
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    sort(allRecord.begin(),allRecord.end(),lost::lostTimeComp);
    if(this->isSortByLostDesc){
        reverse(allRecord.begin(),allRecord.end());
    }
    this->isSortByLostDesc = !this->isSortByLostDesc;
    this->update(allRecord);
}

void dashboard::on_sortByFound_clicked()
{
    // 点击按拾取时间排序
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    sort(allRecord.begin(),allRecord.end(),lost::pickUpTimeComp);
    if(this->isSortByFoundDesc){
        reverse(allRecord.begin(),allRecord.end());
    }
    this->isSortByFoundDesc = !this->isSortByFoundDesc;
    this->update(allRecord);
}

void dashboard::on_exportRecord_clicked()
{
    // 点击导出文本
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    time_t t=time(0);
    tm *ltm = localtime(&t);
    QDateTime datetime;
    QString timestr=datetime.currentDateTime().toString("yyyy-MM-dd");//确定输出格式
    string tst = timestr.toStdString();
    Util::generater(allRecord,tst);

    QMessageBox::about(NULL, "导出成功", "所有遗失物品记录已成功导出文件！");

}

void dashboard::on_export_last_week_clicked()//导出一周记录
{
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    vector<lost> lastWeek;
    for(int i=0,length=allRecord.size();i<length;i++){
        if(allRecord[i].isLastWeekLost()){
            lastWeek.push_back(allRecord[i]);
        }
    }
    Util::generater(lastWeek,"last_week");
    QMessageBox::about(NULL, "导出成功", "上周遗失物品记录已成功导出文件！");
    system("last_week.txt");
}

void dashboard::on_export_last_year_clicked()//导出一年记录
{
    vector<lost> allRecord = Util::getStorageSync<lost>("record");
    vector<lost> lastYear;
    for(int i=0,length=allRecord.size();i<length;i++){
        if(allRecord[i].isLastYearLost()){
            lastYear.push_back(allRecord[i]);
        }
    }
    Util::generater(lastYear,"c");
    QMessageBox::about(NULL, "导出成功", "一年以上的遗失物品记录已成功导出文件！");
    system("years_up.txt");
}



//查找功能
void dashboard::show_findText()
{
    QString findText = find_textLineEdit->text();//判定是否为空
    if (findText!=""){
        vector<lost> allRecord = Util::getStorageSync<lost>("record");
        for(int i = 0; i < keyWordList.size(); i++)//遍历查找
        {
            QString text = QString::fromStdString(allRecord[i].getName());
            if((findText.contains(text) || text.contains(findText)) && (text!=""))//在丢失名中查找
            {
                if(currentIndex < i){
                    currentIndex = i;
                    break;
                }
            }
            text = QString::fromStdString(allRecord[i].getCategory());
            if((findText.contains(text) || text.contains(findText)) && (text!=""))//类型查找
            {
                if(currentIndex < i){
                    currentIndex = i;
                    break;
                }
            }
            text = QString::fromStdString(allRecord[i].getLostDay());//日期（输入区间特性为月份）
            if((findText.contains(text) || text.contains(findText)) && (text!=""))
            {
                if(currentIndex < i){
                    currentIndex = i;
                    break;
                }
            }
            text = QString::fromStdString(allRecord[i].getPlace());
            if((findText.contains(text) || text.contains(findText)) && (text!=""))
            {
                if(currentIndex < i){
                    currentIndex = i;
                    break;
                }
            }
            text = QString::fromStdString(allRecord[i].getStuId());
            if((findText.contains(text) || text.contains(findText)) && (text!=""))
            {
                if(currentIndex < i){
                    currentIndex = i;
                    break;
                }
            }
            text = QString::fromStdString(allRecord[i].getStuName());
            if((findText.contains(text) || text.contains(findText)) && (text!=""))
            {
                if(currentIndex < i){
                    currentIndex = i;
                    break;
                }
            }


        }

        if(currentIndex >=0)
            ui->recordTable->selectRow(currentIndex);
        //以下判断，循环查找使用
        if( lastIndex == currentIndex)
        {
          lastIndex = currentIndex = -1;
        }else{
            lastIndex = currentIndex;
        }
    }else{
        QMessageBox::warning(NULL,"无法搜索","请给出搜索参数");
    }


}
//查找

void dashboard::on_pushButton_released()
{
    keyWordList.clear();
    currentIndex = -1;
    lastIndex = -1;
    for(int i = 0; i < ui->recordTable->rowCount(); i++)
    {
        keyWordList << ui->recordTable->item(i,0)->text();
    }
    findDlg ->show();
}



