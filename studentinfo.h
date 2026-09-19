#ifndef STUDENTINFO_H
#define STUDENTINFO_H

#include <QString>
#include <QVector>

// ============================================================================
//  在这里填写你的个人信息 —— About 对话框显示的内容全部取自这个类。
//
//  两种修改方式，任选一种：
//    1) 直接改下面的 kDefaults() 里的字符串，重新编译；
//    2) 不改代码，直接编辑可执行文件同目录下的 test/about.json，无需重新编译。
//       文件格式示例：
//       {
//           "title":       "关于本程序",
//           "application": "示例 2.4 混合方式 UI 设计",
//           "name":        "张三",
//           "studentId":   "20230001",
//           "className":   "软件工程三班",
//           "major":       "软件工程",
//           "school":      "计算机学院"
//       }
//       （缺少的字段会自动回退到下面默认值。）
// ============================================================================

struct StudentInfo
{
    QString title;         // 对话框标题
    QString application;   // 课程示例名称
    QString name;          // 姓名
    QString studentId;     // 学号
    QString className;     // 班级
    QString major;         // 专业
    QString school;        // 学校 / 学院

    // 内置默认值（找不到 test/about.json 时使用）
    static StudentInfo defaults();

    // 从 JSON 文件读取，文件不存在或格式错误时返回 defaults()
    static StudentInfo fromJsonFile(const QString &filePath);

    // 依次尝试：可执行文件目录/test/about.json、当前目录/test/about.json
    static StudentInfo load();

    // 用于调试输出
    QString toString() const;
};

#endif // STUDENTINFO_H
