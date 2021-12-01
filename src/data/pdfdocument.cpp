#include "pdfdocument.h"

#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QApplication>
#include <QClipboard>

#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-image.h>

using namespace poppler;

PdfDocument::PdfDocument(QString file, QObject *parent) : QObject(parent)
{
    _doc = document::load_from_file(file.toStdString());
    if (_doc != nullptr && _doc->pages() > 0) {
        _page = _doc->create_page(0);
    } else {
        _page = nullptr;
        _renderer = nullptr;
    }
    // use loadFromData to avoid holding a lock on the PDF file in windows
/*    QFile f(file);
    if (f.open(QFile::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();
        // _doc = Poppler::Document::loadFromData(data);
    } else {
        _doc = nullptr;
    }

    if (!_doc) {
        _doc = nullptr;
        _page = nullptr;
    } else {
        // _doc->setRenderHint(Poppler::Document::Antialiasing);
        // _doc->setRenderHint(Poppler::Document::TextAntialiasing);
        // _doc->setRenderHint(Poppler::Document::TextHinting);
        // _page = _doc->page(0);
    }*/
}

void PdfDocument::renderTo(QLabel *label, QRect rect)
{
    if (!isValid()) return;
    qreal w0 = _page->page_rect().right() - _page->page_rect().left();
    qreal h0 = _page->page_rect().bottom() - _page->page_rect().top();
    qreal ratio = label->devicePixelRatioF();
    int w = static_cast<int>(ratio * (rect.width() - 20));
    int h = static_cast<int>(ratio * (rect.height() - 20));

    // not all platforms have fmin, compute the min by hand
    qreal hscale = static_cast<qreal>(w) / w0;
    qreal vscale = static_cast<qreal>(h) / h0;
    qreal scale = (hscale < vscale) ? hscale : vscale;

    int dpi = static_cast<int>(scale * 72.0);
    int w1 = static_cast<int>(scale * w0);
    int h1 = static_cast<int>(scale * h0);

    page_renderer renderer;
    renderer.set_render_hint(page_renderer::render_hint::antialiasing, true);
    renderer.set_render_hint(page_renderer::render_hint::text_antialiasing, true);
    renderer.set_render_hint(page_renderer::render_hint::text_hinting, true);
    image img = renderer.render_page(_page, dpi, dpi, (w1 - w)/2,  (h1 - h)/2, w, h);
/*
    QSizeF pageSize = _page->pageSizeF();

    qreal ratio = label->devicePixelRatioF();
    int w = static_cast<int>(ratio * (rect.width() - 20));
    int h = static_cast<int>(ratio * (rect.height() - 20));

    // not all platforms have fmin, compute the min by hand
    qreal hscale = static_cast<qreal>(w) / pageSize.width();
    qreal vscale = static_cast<qreal>(h) / pageSize.height();
    qreal scale = (hscale < vscale) ? hscale : vscale;

    int dpi = static_cast<int>(scale * 72.0);
    int w1 = static_cast<int>(scale * pageSize.width());
    int h1 = static_cast<int>(scale * pageSize.height());

    //qDebug() << "hidpi ratio:" << ratio;
    //qDebug() << "visible width:" << w;
    //qDebug() << "visible height:" << h;
    //qDebug() << "doc width:" << pageSize.width();
    //qDebug() << "doc height:" << pageSize.height();
    //qDebug() << "scale:" << scale;
    //qDebug() << "dpi:" << dpi;

    QPixmap pm = QPixmap::fromImage(_page->renderToImage(dpi, dpi, (w1 - w)/2,  (h1 - h)/2, w, h));
    pm.setDevicePixelRatio(ratio);
    label->setPixmap(pm);
    */
}

bool PdfDocument::isValid()
{
    return (_page != nullptr);
}

bool PdfDocument::exportImage(QString file, const char *format, QSize outputSize)
{
    QImage img = asImage(outputSize);
    if (!img.isNull()) return img.save(file, format);
    else return false;
}

bool PdfDocument::exportPdf(QString file)
{
    if (!isValid()) return false;
    return false;
    /*std::unique_ptr<Poppler::PDFConverter> conv = _doc->pdfConverter();
    conv->setOutputFileName(file);
    bool success = conv->convert();
    return success;*/
}

void PdfDocument::copyImageToClipboard(QSize outputSize)
{
    QImage img = asImage(outputSize);
    if (!img.isNull()) {
        QApplication::clipboard()->setImage(img, QClipboard::Clipboard);
    }
}

QImage PdfDocument::asImage(QSize outputSize)
{
    return QImage();
    /*if (!isValid()) return QImage();
    if (outputSize.isNull()) outputSize = size();
    QSize pageSize = _page->pageSize();
    int dpix = (72 * outputSize.width()) / pageSize.width();
    int dpiy = (72 * outputSize.width()) / pageSize.width();
    QImage img = _page->renderToImage(dpix, dpiy, 0,  0,
                                      outputSize.width(), outputSize.height());
    return img;*/
}

// CRASHES TikZiT when figures contain text, due to limitations of Arthur backend
//void PdfDocument::exportToSvg(QString file, QSize size) {
//    QSvgGenerator gen;
//    gen.setFileName(file);
//    gen.setSize(size);
//    gen.setViewBox(QRect(0,0,size.width(),size.height()));
//    gen.setDescription("SVG generated from PDF by TikZiT");
//    QPainter painter;

//    // set the backend to Qt for renderToPainter() support
//    Poppler::Document::RenderBackend backend = _doc->renderBackend();
//    _doc->setRenderBackend(Poppler::Document::ArthurBackend);
//    painter.begin(&gen);
//    _page->renderToPainter(&painter);
//    painter.end();
//    _doc->setRenderBackend(backend);
//}

QSize PdfDocument::size()
{
    if (isValid()) {
        return QSize();//_page->pageSize();
    } else {
        return QSize();
    }
}


