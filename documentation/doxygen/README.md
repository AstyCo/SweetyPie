<div class="the_markdown">
# README

QtGantt - проект диаграммы Ганта, следующий QT паттерну MVD.
Зависит от extensions-lib

## Сводка по include

| File | Information |
| ------ | ------ |
| **ganttmodel/gantt/factory/ganttfactory.h(.cpp)** | объявление(реализация) GanttFactory |
| **extensions-gui/interfaces/imodelwrapper.h(.cpp)** | объявление(реализацию) IModelWrapper |
| **extensions-gui/interfaces/abstractganttfactory.h(.cpp)** | объявление(реализацию) AbstractGanttFactory|

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
    
**ModelWrapper** реализует **IModelWrapper**

```cpp
    // --- Interface

    /*!
     * \brief model
     * \return Указатель на QAbstractItemModel class
     */
    virtual QAbstractItemModel *model() const = 0;
    /*!
     * \brief tag
     * \param index
     * \return Тип события
     */
    virtual QString     tag(const QModelIndex &index) const = 0;
    /*!
     * \brief title
     * \param iGanttIndex
     * \return Имя события
     */
    virtual QString     title(const QModelIndex &iGanttIndex) const = 0;
    /*!
     * \brief start
     * \param iGanttIndex
     * \return Начало события
     */
    virtual UtcDateTime start(const QModelIndex &iGanttIndex) const = 0;
    /*!
     * \brief length
     * \param iGanttIndex
     * \return Длительность события
     */
    virtual TimeSpan    length(const QModelIndex &iGanttIndex) const = 0;
    /*!
     * \brief color
     * \param iGanttIndex
     * \return Цвет для события
     */
    virtual QColor      color(const QModelIndex &iGanttIndex) const = 0;

    // ---
    /*!
     * \brief isEvent
     * \return Отслеживать событие
     */
    virtual bool        isEvent(const QModelIndex &/*index*/) const;
```
Смотри **gantt-full-demo** / **ganttmodel-test**
### Вариант 2
Данный вариант требует от программиста большего количества работы, но
предоставляет большую гибкость.
Можно реализовать **AbstractGanttFactory**: методы получения объектов
сцены (производных от GanttGraphicsObject, либо от GanttTextGraphicsObject, 
следует выбрать второй случай для объектов, подразумевающих отображение текста
на диаграмме Гантта)
    
GanttFactory реализует AbstractGanttFactory
```cpp
    /*!
     * \brief createInfo
     * \param index
     * \param parent
     * \return GanttInfoItem class, соотнесенный с index, дочерний для parent
     */
    virtual GanttInfoItem *createInfo(const QModelIndex &index, GanttInfoItem *parent) = 0;

    /*!
     * \brief createGraphicsObject
     * \param info
     * \param index
     * \return Объект сцены соотнесенный с info, index
     */
    virtual GanttGraphicsObject *createGraphicsObject(GanttInfoItem *info, const QModelIndex &index) = 0;

    /*!
     * \brief model
     * \return Указатель на конкретную QAbstractItemModel class
     */
    virtual QAbstractItemModel *model() const = 0;


    virtual GanttInfoItem *infoForIndex(const QModelIndex &index) const = 0;
    /*!
     * \brief virtual indexForInfo
     *
     * Переопределите, чтобы диаграмма могла работать с данными представленными в специальном виде.
     * \param item Указатель на GanttInfoItem class
     * \return QModelIndex class для GanttInfoItem class
     */
    virtual QModelIndex indexForInfo(const GanttInfoItem *item) const = 0;
    virtual bool isEvent(const QModelIndex &index) const;
    virtual void initialize(GanttInfoTree *tree = NULL);
```
</div>