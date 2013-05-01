RfidTap
=======
Web server that lets you use AJAX to obtain RFID numbers from the Sparkfun RFID USB Reader.

Based on the ID-12 connected to the [Sparkfun RFID USB Reader](https://www.sparkfun.com/products/9963).

Uses [server-side access control](https://developer.mozilla.org/en-US/docs/Server-Side_Access_Control) to allow
cross-site AJAX requests, avoiding having to use JSONP.

License
-------
MIT License. See LICENSE.

Technical Details
-----------------
See Wiki.

Example jQuery Code
-------------------
    $.ajax("http://localhost:4567/rfid", {
            dataType: "text",
            success: function(rfid) {
                console.log("Got RFID: " + rfid);
            },
            error: function() {
                console.warn("Could not connect to RfidTap server");
            },
            jsonp: false
    });
