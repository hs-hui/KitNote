# SPDX-License-Identifier: GPL-3.0-only
# KitNote: what AI tools use - the skill (SKILL.md), the audio engine (kitnote_audio.py) and its installer.

# the audio engine's installer: PowerShell on Windows, sh on a Mac (KitNote runs it after the user has agreed)
if (OS_IS_MAC)
    set(KITNOTE_ENGINE_INSTALLER install_engine.sh)
else()
    set(KITNOTE_ENGINE_INSTALLER install_engine.ps1)
endif()

install(FILES
      SKILL.md
      kitnote_audio.py
      kitnote_drum_model.py
      kitnote_drums.NOTICE.txt
      ${KITNOTE_ENGINE_INSTALLER}
      DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}ai
      )
# KitNote's drum model (made by tools/kitnote/train_drum_model.py): installed when it is there
install(FILES kitnote_drums.pt DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}ai OPTIONAL)
