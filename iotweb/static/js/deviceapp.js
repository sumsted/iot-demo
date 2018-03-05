var app = {
    'init': function () {
        $("#happy-icon").on("click", app.postHappy);
        $("#meh-icon").on("click", app.postMeh);
        $("#sad-icon").on("click", app.postSad);
    },
    'postDevice': function (kind) {
        var url = "/" + queueName + "/peek";
        var id = "#" + containerId;
        $.getJSON(
            url
        ).done(function (data) {
            $(id).html("");
            if (data.data.length > 0) {
                var colorIndex = 0;
                $.each(data.data, function (index, item) {
                    var colorCode = "#" + Array(7).join(colorIndex.toString(16));
                    colorIndex++;
                    var result = "<div class=\"row\"><div class=\"col-md\" style=\"color:"+colorCode+"\">" +
                        JSON.stringify(item) +
                        "</div></div>";
                    $(id).append(result);
                });
            }
        });
    },
    'postHappy': function () {
        app.postDevice("happy");
    },
    'postMeh': function () {
        app.postDevice("meh");
    },
    'postSad': function () {
        app.postDevice("sad");
    }
}

app.init();
