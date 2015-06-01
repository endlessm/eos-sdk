#!/usr/bin/python
import json
import os
import time
from gi.repository import Gio, Gtk, GLib

DIRECTORY_ID = 'eos-folder-user-99.directory'
DIRECTORY_FILE_CONTENT="""[Desktop Entry]
Name=Development
Icon=eos-folder-tools-symbolic
Type=Directory
"""

DEFAULT_LAYOUT_FILE='/usr/share/EndlessOS/language-defaults/icon-grid-C.json'
DESTINATIION_FOLDER= os.path.expanduser('~/.local/share/desktop-directories/') + DIRECTORY_ID

def get_default_layout():
    with open(DEFAULT_LAYOUT_FILE, 'r') as f:
        return f.read()


def write_folder_data():
    with open(DESTINATIION_FOLDER, 'w') as f:
        return f.write(DIRECTORY_FILE_CONTENT)

def get_settings_string(layout):
    settings = Gio.Settings.new("org.gnome.shell")


    loaded_layout = json.loads(layout)
    loaded_layout['desktop'].append(DIRECTORY_ID)
    loaded_layout[DIRECTORY_ID] = [ 'eos-app-gnome-terminal.desktop' ,
                                    'eos-app-devhelp.desktop' ]

    root=GLib.Variant('a{sas}', loaded_layout)
    print "Setting the value"
    settings.set_value("icon-grid-layout", root)
    print "Syncing"
    settings.apply()
    settings.sync()
    print root

    time.sleep(20)

if __name__ == "__main__":
    layout = get_default_layout()
    write_folder_data()
    get_settings_string(layout)
