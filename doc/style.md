Sigyn Style Guide
=================

1. C Guide
-------

- **Tabs** should be spaces, not the \t character. One tab should equal four spaces.
- **Parenthesis** should be seperated by a space between conditionals (if, while, et cetera). There should not be a space between a standard function and parenthesis. <pre><code>if (x != y)
{
&nbsp;&nbsp;&nbsp;&nbsp;function(a, b);
}
</pre></code>

- **Comments** are to use standard C formatting (/* blah */).
- **Functions** are to be formatted as: <pre><code>int function(int count, char fill)
{
&nbsp;&nbsp;&nbsp;&nbsp;blah blah blah;
}
</code></pre>
- **Pointers** are formatted as <code>type *var;</code>
- All **public functions** (Read: Non-staticly declared) must be commented in the below format. All asterisks come one space after the beginning of the line, and a space seperates the asterisk 
    from the rest of the data. If there are no arguments/is no return value, replace the contents of that section with "None". Comments should look like the below:

<pre><code>
    /*
     * Routine Description:
     * Description here.
     * Continuing on multiple lines if needed.
     *
     * Arguments:
     *     One argument - What is is and what is is for.
     *     Another argument - Arguments are one 4-space tab after the line start,
     *
     * Return value:
     *     Whatever your return value is and information about it.
     */
</pre></code>

2. Python Guide
---------------
