const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;


function getSubdirectoryWithLocale(theDir) {

    let locales = GLib.get_language_names();
    let dir = Gio.File.new_for_path(theDir);
    let localeSegment = 'C';

    for(let i=0; i<locales.length; i++) {
        let currLocale = dir.get_child(locales[i]);
        if(currLocale.query_exists(null)) {
            localeSegment = locales[i];
            break;
        }
    }
    return theDir + localeSegment + '/';
}