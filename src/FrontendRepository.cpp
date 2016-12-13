#include "FrontendRepository.h"

#include <llvm/Support/DynamicLibrary.h>

#include "Jit.h"

typedef void*(*frontend_getter)();

namespace jit
{
  Frontend * FrontendRepository::getOrLoad(std::string & name)
  {
    auto frontend = s_Frontends.find(name);
    if (frontend != s_Frontends.end())
    {
      return frontend->second;
    }


    Frontend * new_front = nullptr;

    // Check all paths for the library. Load the first one found
    for (auto it = m_Jit->getFrontendPathBegin(); it != m_Jit->getFrontendPathEnd(); ++it)
    {
      auto path = *it + "/" + name;

      // TODO: Find a better way to check if the frontend exists, instead of trying to load it at each path,
      // and trying to find the symbol
      llvm::sys::DynamicLibrary::LoadLibraryPermanently(path.c_str());
      frontend_getter constructor = (frontend_getter)(llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(("frontend_" + name).c_str()));

      if (constructor)
      {
        new_front = static_cast<Frontend*>(constructor());
        break;
      }
    }

    // If we found the symbol, then the library was loaded correctly
    if (new_front != nullptr)
    {
      s_Frontends[name] = new_front;
      return new_front;
    }
    return nullptr;
  }
}
