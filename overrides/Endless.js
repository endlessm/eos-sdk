const GLib = imports.gi.GLib;
const Gio = imports.gi.Gio;

let Endless;

// Returns the directory that the currently executing JS file resides in
// This is a silly hack that creates an error and looks at its stack trace
function getCurrentFileDir() {
    let e = new Error();
    let caller = e.stack.split('\n')[1];
    let pathAndLine = caller.split('@')[1];
    let path = pathAndLine.split(':')[0];
    let file = Gio.File.new_for_path(path);
    while (GLib.file_test(file.get_path(), GLib.FileTest.IS_SYMLINK)) {
        let link_path = GLib.file_read_link(file.get_path());
        // link_path may be relative, we need to resolve it from current dir
        file = file.get_parent().resolve_relative_path(link_path);
    }

    // Get full path from GIO
    return file.get_parent().get_path();
}

imports.searchPath.unshift(getCurrentFileDir());

const AssetButton = imports.endless_private.asset_button;
const SearchBox = imports.endless_private.search_box;
const TopbarHomeButton = imports.endless_private.topbar_home_button;
const TopbarNavButton = imports.endless_private.topbar_nav_button;

function _init() {
    // this is imports.gi.Endless
    Endless = this;
    Endless.getCurrentFileDir = getCurrentFileDir;
    Endless.AssetButton = AssetButton.AssetButton;
    Endless.SearchBox = SearchBox.SearchBox;
    Endless.TopbarHomeButton = TopbarHomeButton.TopbarHomeButton;
    Endless.TopbarNavButton = TopbarNavButton.TopbarNavButton;

    // Override Endless.PageManager.add() so that you can set child properties
    // at the same time
    Endless.PageManager.prototype._add_real = Endless.PageManager.prototype.add;
    Endless.PageManager.prototype.add = function(child, props) {
        this._add_real(child);
        if(typeof(props) !== 'undefined') {
            for(let prop_id in props) {
                this.child_set_property(child, prop_id, props[prop_id]);
            }
        }
    };

    // Override Endless.ProfileProbe.start() to populate it with the location
    // in the JS file, like the EOS_PROFILE_PROBE pre-processor macro does for
    // C files
    Endless.ProfileProbe._start_real = Endless.ProfileProbe.start;
    Endless.ProfileProbe.start = function(name) {
        let exc = new Error();
        let splits = exc.stack.split('\n')[1].split(':');
        let [line] = splits.slice(-2);
        let loc = splits.slice(0, -2).join(':');
        let [func, file] = loc.split('@');
        if (func === '') {
            func = '<main>';
        }
        return Endless.ProfileProbe._start_real(file, line, func, name);
    };
}
