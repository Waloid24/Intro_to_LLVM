# Intro_to_LLVM

## Several LLVM instructions
- _**`trunc`**_
 
    	<result> = trunc <ty> <value> to <ty2>             ; yields ty2
    
    The `trunc` instruction truncates its operand to the type ty2.
    
	An example in [Godbolt](https://godbolt.org/z/4qEfxEq1Y)

- _**`getelementptr`**_

        <result> = extractvalue <aggregate type> <val>, <idx>{, <idx>}*
    
    The `extractvalue` instruction extracts the value of a member field from an aggregate value.

    An example in [Godbolt](https://godbolt.org/z/7E36o7Mhv)

- _**`srem`**_

        <result> = srem <ty> <op1>, <op2>   ; yields ty:result

    The `srem` instruction returns the remainder from the signed division of its two operands.

    An example in [Godbolt](https://godbolt.org/z/TWcf85rjj)
    

- _**`lshr`**_

        <result> = lshr <ty> <op1>, <op2>         ; yields ty:result

    The `lshr` instruction (logical shift right) returns the first operand shifted to the right a specified number of bits with zero fill.

    An example in [Godbolt](https://godbolt.org/z/EK47ssr3G)


- _**`zext`**_

        <result> = zext <ty> <value> to <ty2>             ; yields ty2
    
    The `zext` instruction zero extends its operand to type ty2.

    An example in [Godbolt](https://godbolt.org/z/saGKTM15f)

- _**`sitofp`**_

        <result> = sitofp <ty> <value> to <ty2>             ; yields ty2

    The `sitofp` instruction regards value as a signed integer and converts that value to the ty2 type.

    An example in [Godbolt](https://godbolt.org/z/5j31T3GdT)

- _**`select`**_

        <result> = select [fast-math flags] selty <cond>, <ty> <val1>, <ty> <val2>             ; yields ty

    The `select` instruction is used to choose one value based on a condition, without IR-level branching.

    An example in [Godbolt](https://godbolt.org/z/Y61sK5o3P)

- _**`fpext`**_

        <result> = fpext <ty> <value> to <ty2>             ; yields ty2

    The ‘fpext’ extends a floating-point value to a larger floating-point value.
    An example in [Godbolt](https://godbolt.org/z/5PGaTbzr5)

- _**`tail call`**_

        <result> = tail call [fast-math flags] [cconv] [ret attrs] [addrspace(<num>)] <ty>|<fnty> <fnptrval>(<function args>) [fn attrs] [ operand bundles ]

    It is used to indicate the tail call of a function.

    An example in [Godbolt](https://godbolt.org/z/GfYfWKf3a)