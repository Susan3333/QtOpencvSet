#pragma once

#include <QWidget>
#include "ui_Gaussian_filter_UI.h"

class Gaussian_filter_UI : public QWidget
{
	Q_OBJECT

public:
	Gaussian_filter_UI(QWidget *parent = Q_NULLPTR);
	~Gaussian_filter_UI();

private:
	Ui::Gaussian_filter_UI ui;
};
