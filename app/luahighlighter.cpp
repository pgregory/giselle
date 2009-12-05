#include <QtGui>

#include "luahighlighter.h"

LuaHighlighter::LuaHighlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    QStringList keywordPatterns;

    keywordFormat0.setForeground(Qt::darkBlue);
    keywordFormat0.setFontWeight(QFont::Bold);
    keywordPatterns  << "\\band\\b" << "\\bbreak\\b" << "\\bdo\\b"
                     << "\\belse\\b" << "\\belseif\\b" << "\\bend\\b"
                     << "\\bfalse\\b" << "\\bfor\\b" << "\\bfunction\\b"
                     << "\\bif\\b" << "\\bin\\b" << "\\blocal\\b"
                     << "\\bnil\\b" << "\\bnot\\b" << "\\bor\\b"
                     << "\\brepeat\\b" << "\\breturn\\b" << "\\bthen\\b"
                     << "\\btrue\\b" << "\\buntil\\b" << "\\bwhile\\b";
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat0;
        highlightingRules.append(rule);
    }

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);


    keywordFormat1.setForeground(Qt::darkBlue);
    keywordFormat1.setFontWeight(QFont::Bold);
    keywordPatterns.clear();
    keywordPatterns  << "_VERSION" << "assert" << "collectgarbage"
                     << "dofile" << "error" << "gcinfo"
                     << "loadfile" << "loadstring" << "print"
                     << "rawget" << "rawset" << "require"
                     << "tonumber" << "tostring" << "type"
                     << "unpack";
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat1;
        highlightingRules.append(rule);
    }

    keywordFormat2.setForeground(Qt::darkBlue);
    keywordFormat2.setFontWeight(QFont::Bold);
    keywordPatterns.clear();
    keywordPatterns  << "_G" << "getfenv" << "getmetatable"
                     << "ipairs" << "loadlib" << "next"
                     << "pairs" << "pcall" << "rawequal"
                     << "setfenv" << "setmetatable" << "xpcall"
                     << "string" << "table" << "math"
                     << "coroutine" << "io" << "os" << "debug"
                     << "load" << "module" << "select";
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat2;
        highlightingRules.append(rule);
    }


    keywordFormat3.setForeground(Qt::darkBlue);
    keywordFormat3.setFontWeight(QFont::Bold);
    keywordPatterns.clear();
    keywordPatterns << "string.byte" << "string.char" << "string.dump" << "string.find" << "string.len"
                    << "string.lower" << "string.rep" << "string.sub" << "string.upper" << "string.format" << "string.gfind" << "string.gsub"
                    << "table.concat" << "table.foreach" << "table.foreachi" << "table.getn" << "table.sort" << "table.insert" << "table.remove" << "table.setn"
                    << "math.abs" << "math.acos" << "math.asin" << "math.atan" << "math.atan2" << "math.ceil" << "math.cos" << "math.deg" << "math.exp"
                    << "math.floor" << "math.frexp" << "math.ldexp" << "math.log" << "math.log10" << "math.max" << "math.min" << "math.mod"
                    << "math.pi" << "math.pow" << "math.rad" << "math.random" << "math.randomseed" << "math.sin" << "math.sqrt" << "math.tan"
                    << "string.gmatch" << "string.match" << "string.reverse" << "table.maxn"
                    << "math.cosh" << "math.fmod" << "math.modf" << "math.sinh" << "math.tanh" << "math.huge";
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat3;
        highlightingRules.append(rule);
    }


    keywordFormat4.setForeground(Qt::darkBlue);
    keywordFormat4.setFontWeight(QFont::Bold);
    keywordPatterns.clear();
    keywordPatterns << "coroutine.create" << "coroutine.resume" << "coroutine.status"
                    << "coroutine.wrap" << "coroutine.yield"
                    << "io.close" << "io.flush" << "io.input" << "io.lines" << "io.open" << "io.output" << "io.read" << "io.tmpfile" << "io.type" << "io.write"
                    << "io.stdin" << "io.stdout" << "io.stderr"
                    << "os.clock" << "os.date" << "os.difftime" << "os.execute" << "os.exit" << "os.getenv" << "os.remove" << "os.rename"
                    << "os.setlocale" << "os.time" << "os.tmpname"
                    << "coroutine.running" << "package.cpath" << "package.loaded" << "package.loadlib" << "package.path"
                    << "package.preload" << "package.seeall" << "io.popen"
                    << "debug.debug" << "debug.getfenv" << "debug.gethook" << "debug.getinfo" << "debug.getlocal"
                    << "debug.getmetatable" << "debug.getregistry" << "debug.getupvalue" << "debug.setfenv"
                    << "debug.sethook" << "debug.setlocal" << "debug.setmetatable" << "debug.setupvalue" << "debug.traceback";
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat4;
        highlightingRules.append(rule);
    }

    renderableFormat.setForeground(Qt::darkGreen);
    renderableFormat.setFontWeight(QFont::Bold);
    QStringList renderablePatterns;
    renderablePatterns << "WorldBegin" << "WorldEnd" << "TransformBegin" << "TransformEnd"
                       << "Translate" << "Rotate" << "Scale"
                       << "Cylinder" << "Sphere" << "Disk" << "PatchMesh"
                       << "Polygon"
                       << "Projection"
                       << "CoordinateSystem" << "CoordSysTransform"
                       << "RecordTransform" << "RestoreTransform";

    foreach (const QString &pattern, renderablePatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = renderableFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("--[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("--[[");
    commentEndExpression = QRegExp("]]--");
 }

void LuaHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                             + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}


