const Gio = imports.gi.Gio;

let Endless;

// Returns the directory that the currently executing JS file resides in
// This is a silly hack that creates an error and looks at its stack trace
function getCurrentFileDir() {
    let e = new Error();
    let caller = e.stack.split('\n')[1];
    let pathAndLine = caller.split('@')[1];
    let path = pathAndLine.split(':')[0];

    // Get full path from GIO
    return Gio.File.new_for_path(path).get_parent().get_path();
}

function _init() {
    // this is imports.gi.Endless
    Endless = this;
    Endless.getCurrentFileDir = getCurrentFileDir;

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
    }
}
