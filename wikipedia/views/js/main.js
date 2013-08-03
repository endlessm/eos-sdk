window.Endless = window.Endless || {};
Endless.Wikipedia = Endless.Wikipedia || {};

(function(window, document, $) {

    "use strict";

    Endless.Wikipedia.App = function() {
        var privateObject,
            $body = $("body"),
            $image = $(".image"),
            $main = $("#main"),
            $main_wrapper_home = $("#main-wrapper-home"),
            $main_results = $("#main-results"),
            $main_wrapper = $("#main-wrapper"),
            $main_content = $("#main-content"),
            $inside_content = $("#inside-content"),
            $header_content = $("#header-content"),
            $main_wrapper_middle = $("#main-wrapper-middle"),
            $wiki_content = $("#wiki_content"),
            lang = "pt",

            _resultPageView = function(pageHtml, titlePage, resultError) {
                if (resultError === false) {
                    $inside_content.html(pageHtml);
                    $header_content.html('<h1>' + titlePage + '</h1>');
                    $('#inside-content').prepend('<hr class="hr-title">');

                    $('#inside-content [src^="//"]').each(function() {
                        var parent = $(this).parent();
                        if($(this).is("img")){
                            var url = $(this).attr("src");
                            var theSplit = url.split("/");
                            var filename = theSplit[theSplit.length - 2];
                            if(filename.length == 2){
                                filename = theSplit[theSplit.length - 1];
                            }
                            var newSrc = "article_images/" + encodeURI(filename)
                            if(newSrc.substring(newSrc.length - 4) == ".svg"){
                                newSrc += ".png";
                            }
                            $(this).attr('src', newSrc);
                        }
                        
                    });
                }
            },

            wresize = function() {
                $main_wrapper_home.height($(window).height());
                $main_results.height($(window).height());
                $main_wrapper.height($(window).height());
                $main_content.height($(window).height() - $("#header-top").outerHeight());
                $inside_content.height($main_content.outerHeight() - ($("#header-content").outerHeight() + 15));
                $main_wrapper_home.css("background-size", "100% " + $(window).height() + "px");
                $main_results.css("background-size", "100% " + $(window).height() + "px");
            }

        return {
            init: function() {
                $(document).hide();
                $(window).resize(wresize);
                var content = $('#wiki_content').html();
                var title = $('#wiki_content').attr('name');
                _resultPageView(content, title, false);
                $('#wiki_content').remove();
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