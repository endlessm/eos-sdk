const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const CategoryLayoutManager = new Lang.Class({
    Name: 'CategoryLayoutManager',
    Extends: Gtk.Grid,

    _init: function(props) {
        props = props || {};
        props.column_homogeneous = true;
        props.row_homogeneous = true;
        this.parent(props);

        this._childWidgets = [];
    },

    // Distribute children in two columns, except for the last one if an odd
    // number; that should span two columns
    _redistributeChildren: function() {
        let numChildren = this._childWidgets.length;
        let oddNumber = numChildren % 2 == 1;
        this._childWidgets.forEach(function(child, index) {
            let column = index % 2;
            let row = Math.floor(index / 2);

            if(child.get_parent() === this)
                Gtk.Container.prototype.remove.call(this,
                    this._childWidgets[index]);

            if(oddNumber && index == numChildren - 1)
                this.attach(child, 0, row, 2, 1);
            else
                this.attach(child, column, row, 1, 1);
        }, this);
    },

    add: function(child) {
        this._childWidgets.push(child);
        this._redistributeChildren();
    },

    remove: function(child) {
        let index = this._childWidgets.indexOf(child);
        if(index == -1) {
            printerr('Widget', System.addressOf(child),
                'is not contained in CategoryLayoutManager');
            return;
        }
        this._childWidgets.splice(index, 1); // remove
        this._redistributeChildren();
    }
});

// Gtk.init(null);
// let w = new Gtk.Window();
// let g = new CategoryLayoutManager();
// let count = 7;
// for(let i = 0; i < count; i++) {
//     let widget = new Gtk.Button({label: 'Widget ' + i});
//     g.add(widget);
// }
// w.add(g);
// w.connect('destroy', Gtk.main_quit);
// w.show_all();
// Gtk.main();
