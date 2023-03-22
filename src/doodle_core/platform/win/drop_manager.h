//
// Created by TD on 2021/12/31.
//

#include <doodle_core/configure/doodle_core_export.h>
#include <doodle_core/core/file_sys.h>

#include <oleidl.h>

void OpenFilesFromDataObject(IDataObject *pdto);

namespace doodle::facet {
class gui_facet;
}
namespace doodle::win {
class DOODLE_CORE_API ole_guard {
 public:
  ole_guard();
  virtual ~ole_guard();
};

class DOODLE_CORE_API drop_manager : public IDropTarget {
 public:
  using drag_over_fun_type = std::function<void(DWORD grfKeyState, POINTL ptl)>;

 private:
  LONG m_RefCount{};

  bool begin_drop{};
  std::vector<FSys::path> drop_files{};
  drag_over_fun_type drag_over_fun{};

 public:
  drop_manager(drag_over_fun_type in_fun = {})
      : m_RefCount(0), begin_drop(), drop_files(), drag_over_fun(std::move(in_fun)){};
  STDMETHODIMP_(ULONG)
  AddRef() override;

  STDMETHODIMP_(ULONG)
  Release() override;
  //  ~drop_manager();

  STDMETHODIMP QueryInterface(REFIID riid, void **ppv) override;

  // 当我们将文件拖入我们的应用程序视图时发生
  STDMETHODIMP DragEnter(IDataObject *pdto, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;
  // 当我们在携带文件的同时将鼠标拖到我们的应用程序视图上时发生（一直发生）
  STDMETHODIMP DragOver(DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;
  // 当我们从应用程序视图中拖出文件时发生
  STDMETHODIMP DragLeave() override;
  // 当我们释放鼠标按钮完成拖放操作时发生
  STDMETHODIMP Drop(IDataObject *pdto, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;

  [[nodiscard]] explicit operator bool() const { return begin_drop; }

  [[nodiscard]] const std::vector<FSys::path> &GetDropFiles() const;
};

}  // namespace doodle::win
