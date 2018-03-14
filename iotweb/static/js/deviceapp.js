var app = {
    'init': function () {
        $("#happy-icon").on("click", app.postHappy);
        $("#meh-icon").on("click", app.postMeh);
        $("#sad-icon").on("click", app.postSad);
    },
    'postDevice': function (kind) {
        var url = "/iot_gateway_queue/" + web_key;
        var id = "#result";
        var state = {"device": "web01", "state": kind};
        $.ajax(
            {
                url: url,
                data: JSON.stringify(state),
                contentType: 'application/json',
                type: 'POST',
                success: function (data) {
                    $(id).html(data);
                },
                failure: function (err) {
                    $(id).html(err);
                }
            }
        );
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

$().ready(
    function(){
        app.init();
    }
);
