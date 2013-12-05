const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const MAIN_CATEGORY_SCREEN_WIDTH_PERCENTAGE = 37;
const SUB_CATEGORY_SCREEN_WIDTH_PERCENTAGE = 30;

const CategoryLayoutManager = new Lang.Class({
    Name: 'CategoryLayoutManager',
    Extends: Gtk.Grid,

    _init: function(props) {
        props = props || {};
        props.row_homogeneous = true;
        props.column_homogeneous = true;
        this.parent(props);

        this._childWidgets = [];
        this._mainWidget = null;
    },

    // Distribute children in two columns, except for the last one if an odd
    // number; that should span two columns. The width must be divided by
    // percentage according to the spec, so we consider each column in the grid
    // one percent
    _redistributeChildren: function() {
        let numChildren = this._childWidgets.length;
        let oddNumber = numChildren % 2 == 1;

        let numRows = 1;
        this._childWidgets.forEach(function(child, index) {
            let column = (index % 2) * SUB_CATEGORY_SCREEN_WIDTH_PERCENTAGE + MAIN_CATEGORY_SCREEN_WIDTH_PERCENTAGE;
            let row = Math.floor(index / 2);

            if(numRows < row + 1) 
                numRows = row + 1; //our running count of how many rows we have, which we
            //need when we add the main widget.

            if(child.get_parent() === this)
                Gtk.Container.prototype.remove.call(this, this._childWidgets[index]);

            if(oddNumber && index == numChildren - 1)
                this.attach(child, column, row, SUB_CATEGORY_SCREEN_WIDTH_PERCENTAGE * 2, 1);
            else
                this.attach(child, column, row, SUB_CATEGORY_SCREEN_WIDTH_PERCENTAGE, 1);
        }, this);

        if(this._mainWidget) {
            if(this._mainWidget.get_parent() === this) {
                Gtk.Container.prototype.remove.call(this, this._mainWidget);
            }
            this.attach(this._mainWidget, 0, 0, MAIN_CATEGORY_SCREEN_WIDTH_PERCENTAGE, numRows);
        }
    },

    add: function(child) {
        if(child.is_main_category) {
            this._mainWidget = child;
        } else {
            this._childWidgets.push(child);            
        }
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
