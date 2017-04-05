#ifndef ExportImagesWorkOrder_H
#define ExportImagesWorkOrder_H

#include "WorkOrder.h"

#include <functional>

#include <QMutex>

namespace Isis {
  class IException;
  class Image;

  /**
   * @brief Write project images to a user-specified location
   *
   * Takes an imageList and writes it's images to disk at a user-specified location. This works
   *   both with and without context (context menus and file menu).
   *
   *  internalData() stores:
   *    ImageList name [OPTIONAL] - need context if this isn't present (see WorkOrder::imageList())
   *    Output directory name [REQUIRED]
   *
   *
   * @author 2012-09-26 Tracie Sucharski
   *
   * @internal
   */
  class ExportImagesWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ExportImagesWorkOrder(Project *project);
      ExportImagesWorkOrder(const ExportImagesWorkOrder &other);
      ~ExportImagesWorkOrder();

      virtual ExportImagesWorkOrder *clone() const;

      bool isExecutable(ImageList *images);


      bool setupExecution();

      void asyncRedo();
      void postSyncRedo();

    private:
      /**
       * This functor is meant for QtConcurrentMap. This writes images to the output directory and
       *   always returns a void*. This is using map instead of run for performance reasons (map is
       *   much faster). Errors are accumulated, you can call errors() after the map is done to
       *   get them.
       *
       * @author 2012-09-27 Steven Lambright and Tracie Sucharski
       *
       * @internal
       */
      class ProjectImageExportFunctor : public std::unary_function<Image * const &, void *> {
        public:
          ProjectImageExportFunctor(QString destination);
          ProjectImageExportFunctor(const ProjectImageExportFunctor &other);
          ~ProjectImageExportFunctor();

          void *operator()(Image * const &imageToExport);

          IException errors() const;

        private:
          //! Not implemented
          ProjectImageExportFunctor &operator=(const ProjectImageExportFunctor &rhs);

          QString m_destination;

          QMutex m_errorsLock;
          QSharedPointer<IException> m_errors;
          QSharedPointer<int> m_numErrors;
      };

    private:
      ExportImagesWorkOrder &operator=(const ExportImagesWorkOrder &rhs);

      QString m_warning;
  };
}

#endif
