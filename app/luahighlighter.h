#ifndef LUAHIGHLIGHTER_H
#define LUAHIGHLIGHTER_H

#include    <QSyntaxHighlighter>

#include    <QHash>
#include    <QTextCharFormat>

class QTextDocument;

class LuaHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    LuaHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString& text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat renderableFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // LUAHIGHLIGHTER_H
