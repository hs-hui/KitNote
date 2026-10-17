#include "modularity/ioc.h"

#include "fontproviderstub.h"

#include "engraving/dom/score.h"
#include "engraving/compat/scoreaccess.h"

#include "importexport/guitarpro/guitarpromodule.h"

int main()
{
    kfw::modularity::globalIoc()->registerExport<kfw::draw::IFontProvider>("test", new kfw::draw::FontProviderStub());

    kn::engraving::compat::ScoreAccess::createMasterScore(nullptr);

    kn::iex::guitarpro::GuitarProModule gm;
}
