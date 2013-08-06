window.Endless = window.Endless || {};
Endless.Wikipedia = Endless.Wikipedia || {};

(function(window, document, $) {

    "use strict";

    Endless.Wikipedia.App = function() {
        var privateObject,
            $body = $("body"),
            $image = $(".image"),
            $main = $("#main"),
            $main_content = $("#main-content"),
            $inside_content = $("#inside-content"),
            $header_content = $("#header-content"),
            $wiki_content = $("#wiki_content"),
            lang = "pt",

            _resultPageView = function(image_path) {
                $('#inside-content [src^="//"]').each(function() {
                    var parent = $(this).parent();
                    if($(this).is("img")){
                        var url = $(this).attr("src");
                        var theSplit = url.split("/");
                        var filename = theSplit[theSplit.length - 2];
                        if(filename.length == 2){
                            filename = theSplit[theSplit.length - 1];
                        }
                        var newSrc = image_path + encodeURI(filename)
                        if(newSrc.substring(newSrc.length - 4) == ".svg"){
                            newSrc += ".png";
                        }
                        $(this).attr('src', newSrc);
                    } 
                });
                // May need this back at some point
                // $('link[rel=stylesheet]').each(function(){
                //     var src = $(this).attr("href");
                //     console.log(asset_path + src);
                //     $(this).attr("href", asset_path + "/" + src);
                // });
            },

            wresize = function() {
                $main_content.height($(window).height());
                $inside_content.height($main_content.outerHeight() - ($("#header-content").outerHeight() + 15));
            }

        return {
            init: function() {
                $(document).hide();
                var image_path = $inside_content.attr('image_path');
                _resultPageView(image_path);
                wresize();
                $(document).show();
            },
            wresize: wresize
        };
    };

    $(function() {
        Endless.Wikipedia.module = new Endless.Wikipedia.App();
        Endless.Wikipedia.module.init();
    });

}(window, document, jQuery));