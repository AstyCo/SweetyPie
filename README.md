# gantt-lib

Зависит от extensions-lib

## Сводка по include
**ganttmodel/gantt/factory/ganttfactory.h(.cpp)** - объявление(реализация) GanttFactory
**extensions-gui/interfaces/imodelwrapper.h(.cpp)** - объявление(реализацию) IModelWrapper
**extensions-gui/interfaces/abstractganttfactory.h(.cpp)** - объявление(реализацию) AbstractGanttFactory
## How to

Пример использования:
```sh
    CustomItemModel *_model;
    ...
    ui->ganttWidget->setFactory(new GanttFactory(new ModelWrapper(_model)));
    ui->ganttWidget->setView(ui->treeView);
```
### Вариант 1
Можно унаследовать **IModelWrapper**, и использовать стандартную **GanttFactory**, 
тогда будет достаточно описать методы получения информации из **CustomItemModel**
    
*ModelWrapper* реализует **IModelWrapper**

    virtual QAbstractItemModel *model() const = 0;
    virtual QString     tag(const QModelIndex &index) const = 0;  
    virtual QString     title(const QModelIndex &iGanttIndex) const = 0;
    virtual UtcDateTime start(const QModelIndex &iGanttIndex) const = 0;
    virtual TimeSpan    length(const QModelIndex &iGanttIndex) const = 0;
    virtual QColor      color(const QModelIndex &iGanttIndex) const = 0;
    virtual bool        isEvent(const QModelIndex &/*index*/) const;
    
Смотри **gantt-full-demo** / **ganttmodel-test**
### Вариант 2
Данный вариант требует от программиста большего количества работы, но
предоставляет большую гибкость.
Можно реализовать **AbstractGanttFactory**: методы получения объектов
сцены (производных от GanttGraphicsObject, либо от GanttTextGraphicsObject, 
следует выбрать второй случай для объектов, подразумевающих отображение текста
на диаграмме Гантта)
    
GanttFactory реализует AbstractGanttFactory
    virtual GanttInfoItem *createInfo(const QModelIndex &index) = 0;
    virtual GanttGraphicsObject *createGraphicsObject(GanttInfoItem *info) = 0;
    virtual QAbstractItemModel *model() const = 0;
    virtual bool isEvent(GanttInfoItem */*info*/) const;
    
    