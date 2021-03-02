#include "Gaussian_filter_UI.h"

Gaussian_filter_UI::Gaussian_filter_UI(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);
}

Gaussian_filter_UI::~Gaussian_filter_UI()
{
}
