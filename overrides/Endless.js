let Endless;

function _init() {
    // this is imports.gi.Endless
    Endless = this;

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
    
    // Override Endless.ActionMenu.add_action() so that we hide the use of
    // GtkAction from the developer, as that will be deprecated in the future.
    Endless.ActionMenu.prototype._add_action_real = Endless.ActionMenu.prototype.add_action;
    Endless.ActionMenu.prototype.add_action = function(dict, callback) {
	let action = new Gtk.Action(dict);
	this._add_action_real(action);

	if (typeof callback === "function") {
	    action.connect('activate', callback);
	}
    }
}
