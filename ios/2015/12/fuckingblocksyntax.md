


  <link rel="apple-touch-icon" sizes="57x57" href="/apple-touch-icon-57x57.png">
  <link rel="apple-touch-icon" sizes="114x114" href="/apple-touch-icon-114x114.png">
  <link rel="apple-touch-icon" sizes="72x72" href="/apple-touch-icon-72x72.png">
  <link rel="apple-touch-icon" sizes="144x144" href="/apple-touch-icon-144x144.png">
  <link rel="apple-touch-icon" sizes="60x60" href="/apple-touch-icon-60x60.png">
  <link rel="apple-touch-icon" sizes="120x120" href="/apple-touch-icon-120x120.png">
  <link rel="apple-touch-icon" sizes="76x76" href="/apple-touch-icon-76x76.png">
  <link rel="apple-touch-icon" sizes="152x152" href="/apple-touch-icon-152x152.png">
  <link rel="icon" type="image/png" href="/favicon-196x196.png" sizes="196x196">
  <link rel="icon" type="image/png" href="/favicon-160x160.png" sizes="160x160">
  <link rel="icon" type="image/png" href="/favicon-96x96.png" sizes="96x96">
  <link rel="icon" type="image/png" href="/favicon-16x16.png" sizes="16x16">
  <link rel="icon" type="image/png" href="/favicon-32x32.png" sizes="32x32">
</head>

<body>
  <h1>How Do I Declare A Block in Objective-C?</h1>

  <div>
    <h2>As a <strong>local variable</strong>:</h2>
    <code>
      <span class="return">returnType</span> (^<span class="name">blockName</span>)(<span class="parameter-types">parameterTypes</span>) = ^<span class="return">returnType</span>(<span class="parameters">parameters</span>) {...};
    </code>
  </div>

  <div>
    <h2>As a <strong>property</strong>:</h2>
    <code>
      @property (nonatomic, copy) <span class="return">returnType</span> (^<span class="name">blockName</span>)(<span class="parameter-types">parameterTypes</span>);
    </code>
  </div>

  <div>
    <h2>As a <strong>method parameter</strong>:</h2>
    <code>
      - (void)someMethodThatTakesABlock:(<span class="return">returnType</span> (^)(<span class="parameter-types">parameterTypes</span>))<span class="name">blockName</span>;
    </code>
  </div>

  <div>
    <h2>As an <strong>argument to a method call</strong>:</h2>
    <code>
      [someObject someMethodThatTakesABlock:^<span class="return">returnType</span> (<span class="parameters">parameters</span>) {...}];
    </code>
  </div>

  <div>
    <h2>As a <strong>typedef</strong>:</h2>
    <code>
      typedef <span class="return">returnType</span> (^<span class="name">TypeName</span>)(<span class="parameter-types">parameterTypes</span>);<br/>
      <span class="name">TypeName</span> blockName = ^<span class="return">returnType</span>(<span class="parameters">parameters</span>) {...};
    </code>
  </div>

  <div class="disclaimer">
    This site is not intended to be an exhaustive list of all possible uses of blocks.<br>
    If you find yourself needing syntax not listed here, it is likely that a <strong>typedef</strong> would make your code more readable.<br>
    <br>
    Unable to access this site due to the profanity in the URL? <strong><a href="http://goshdarnblocksyntax.com">http://goshdarnblocksyntax.com</a></strong> is a more work-friendly mirror.
  </div>

  <footer>By <a href="http://lazerwalker.com">Mike Lazer-Walker</a>, who has a very bad memory for this sort of thing.</footer>

  <script>
    (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
    (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
    m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
    })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

    ga('create', 'UA-43658906-1', 'fuckingblocksyntax.com');
    ga('send', 'pageview');
  </script>

</body>
</html>
