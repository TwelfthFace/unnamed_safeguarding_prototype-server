#ifndef MY_GRID_LAYOUT_H
#define MY_GRID_LAYOUT_H

#include <QGridLayout>

class my_grid_layout : public QGridLayout
{
public:
    my_grid_layout(QWidget *parent, int max_column_count );
    ~my_grid_layout();

    void add_widget( QWidget* p_widget );

private:
    int m_max_column_count;
};

#endif // MY_GRID_LAYOUT_H
