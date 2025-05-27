// JSON Plugin para Pawn:
// https://github.com/Southclaws/pawn-json

#include <json>


#define callback:%0(%1)						forward %0(%1) ; public %0(%1)


#define MAX_ACTORES 128


enum actorinfo_
{
	Float:_X_,
	Float:_Y_,
	Float:_Z_,
	Float:_A_,
	VW,
	INT,
	SKIN,
	NOMBRE[24],
	ID,
	ROBADO,
	bool:ROBAR,
	STATUS,
	Text3D:LABEL,
	bool:DEAD,
	bool:STEAL,
	ROBADOPOR,
	TIMER
};
new ACTOR[MAX_ACTORS][actorinfo_];
new Iterator: Actores<MAX_ACTORS>;


new ACTOR_COUNT = 0;




CargarActores()
{
    mysql_tquery(conexion, "SELECT * FROM actores", "actors_loading");
}


callback:actors_loading()
{

    new rows = cache_num_rows();
    if (rows == 0)
    {
        printf(" Ultra INF: No se encontraron actores en la base de datos.");
        return 1;
    }
    new jsonData[1024];
    new labels[256],id2 = -1;
    for (new i = 0; i < rows && i < MAX_ACTORS; i++)
    {
        if (cache_get_value_name(i, "DATA", jsonData, sizeof(jsonData)))
        {
            new Node:root;
            if (JSON_Parse(jsonData, root) == 0)  // 0 = exito
            {
                new Float:x, Float:y, Float:z, Float:a;
                if (JSON_GetFloat(root, "X", x) == 0 && JSON_GetFloat(root, "Y", y) == 0 &&JSON_GetFloat(root, "Z", z) == 0 && JSON_GetFloat(root, "A", a) == 0)
                {
                    ACTOR[i][_X_] = x;
                    ACTOR[i][_Y_] = y;
                    ACTOR[i][_Z_] = z;
                    ACTOR[i][_A_] = a;
                    // printf("Verificando actor %d: X=%.4f, Y=%.4f, Z=%.4f, A=%.4f", i, ACTOR[i][_X_], ACTOR[i][_Y_], ACTOR[i][_Z_], ACTOR[i][_A_]);
                }

                JSON_Cleanup(root);
            }
        }
   
   		id2 = Iter_Free(Actores);

        cache_get_value_int(i, "MUNDO",ACTOR[i][VW]);
        cache_get_value_int(i, "INTERIOR",ACTOR[i][INT]);
        cache_get_value_int(i, "SKIN",ACTOR[i][SKIN]);
        cache_get_value_name(i, "NOMBRE", ACTOR[i][NOMBRE]);
        cache_get_value_name(i, "ID", ACTOR[i][ID]);

        format(labels,sizeof(labels),"{FFFFFF}%s {EF6256}[%d]",ACTOR[i][NOMBRE],i);
		ACTOR[i][LABEL] = CreateDynamic3DTextLabel(labels, -1, ACTOR[i][_X_] , ACTOR[i][_Y_] , ACTOR[i][_Z_] + 1, 10.0, .testlos = 1, .interiorid = ACTOR[i][INT], .worldid = ACTOR[i][VW]);
		ACTOR[i][ID] = CreateActor(ACTOR[i][SKIN],ACTOR[i][_X_] , ACTOR[i][_Y_] , ACTOR[i][_Z_] , ACTOR[i][_A_]);

        SetActorInvulnerable(ACTOR[i][ID], false);
        SetActorHealth(ACTOR[i][ID],100);
        SetActorVirtualWorld(ACTOR[i][ID], ACTOR[i][VW]);
        ACTOR[i][ROBADOPOR]=-1;
        ACTOR[i][ROBADO]= 0;
        ACTOR[i][ROBAR] = true;
        ACTOR[i][STEAL]=false;
        ACTOR[i][STATUS]=0;
        ACTOR[i][ROBADOPOR] = -1;
        ACTOR[i][DEAD]=false;
        Iter_Add(Actores, id2);

    }

    ACTOR_COUNT = rows < MAX_ACTORS ? rows : MAX_ACTORS;
    printf(" Ultra INF: %d actores cargados y datos leidos.", ACTOR_COUNT);

    return 1;
}
